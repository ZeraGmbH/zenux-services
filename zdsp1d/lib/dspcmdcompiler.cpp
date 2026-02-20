#include "dspcmdcompiler.h"
#include "dspcompilerrawcollector.h"

DspCmdCompiler::DspCmdCompiler(DspVarResolver *varResolver, int dspInterruptId) :
    m_varResolver(varResolver),
    m_dspInterruptId(dspInterruptId)
{
}

DspCmdWithParamsRaw DspCmdCompiler::compileOneCmdLineZeroAligned(const QString &cmdLine,
                                                                 bool &ok)
{
    return compileOneCmdLineAligned(cmdLine, ok, 0, 0, std::make_shared<DspCompilerRawCollector>());
}

DspCmdWithParamsRaw DspCmdCompiler::compileOneCmdLineAligned(const QString &cmdLine,
                                                             bool &ok,
                                                             ulong userMemOffset,
                                                             ulong globalstartadr,
                                                             AbstractDspCompilerRawCollectorPtr rawCollector)
{
    ok = true;
    cParse CmdParser;
    CmdParser.SetDelimiter("(,)"); // setze die trennzeichen für den parser
    CmdParser.SetWhiteSpace(" (,)");
    if(!syntaxCheck(cmdLine)) {
        ok = false;
        return DspCmdWithParamsRaw();
    }
    const QChar* cmds = cmdLine.data();
    QString sSearch = CmdParser.GetKeyword(&cmds); // das 1. keyword muss ein befehlscode sein
    DspCmdDecodingDetails *dspcmd = DspStaticData::findDspCmd(sSearch);
    if (dspcmd)
    { // bekannter befehlscode ?
        switch (dspcmd->CmdClass) {
        case CMD: // nur kommandowort, kein parameter
        {
            rawCollector->addCmd(dspcmd);
            sSearch = CmdParser.GetKeyword(&cmds);
            ok = sSearch.isEmpty(); // hier darf nichts stehen
            DspCmdWithParamsRaw lcmd;
            if (ok)
                lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode);
            return lcmd;
        }
        case CMD1i16: // kommandowort, ein parameter
        {
            short par;
            sSearch = CmdParser.GetKeyword(&cmds);
            ok &= ( (par = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1); // -1 ist fehlerbedingung
            rawCollector->addCmd1Param(dspcmd, par);
            sSearch = CmdParser.GetKeyword(&cmds);
            ok &= sSearch.isEmpty();
            DspCmdWithParamsRaw lcmd;
            if (ok)
                lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode,(ushort)par);
            return lcmd;
        }
        case CMD2i16:
        {
            short twoI16Params[2];
            QStringList paramNames;
            for (int i=0; i<2; i++) {
                sSearch = CmdParser.GetKeyword(&cmds);
                paramNames.append(sSearch);
                ok &= ( (twoI16Params[i] = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1);
            }
            rawCollector->addCmd2Params(dspcmd, twoI16Params[0], twoI16Params[1]);
            sSearch = CmdParser.GetKeyword(&cmds);
            ok &= sSearch.isEmpty();
            DspCmdWithParamsRaw lcmd;
            if (ok) {
                if (dspcmd->m_cmdExtraCheckFunction)
                    ok = dspcmd->m_cmdExtraCheckFunction(paramNames, twoI16Params, m_varResolver);
                if (ok) {
                    lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode, (ushort)twoI16Params[0], (ushort)twoI16Params[1]);
                    if (dspcmd->modify)
                        lcmd.w[1] = (lcmd.w[1] & 0xFFFF) | (m_dspInterruptId << 16);
                }
            }
            return lcmd;
        }
        case CMD3i16:
        {
            short threeI16Params[3];
            QStringList paramNames;
            for (int i=0; i<3; i++) {
                sSearch = CmdParser.GetKeyword(&cmds);
                paramNames.append(sSearch);
                ok &= ( (threeI16Params[i] = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1);
            }
            rawCollector->addCmd3Params(dspcmd, threeI16Params[0], threeI16Params[1], threeI16Params[2]);
            sSearch = CmdParser.GetKeyword(&cmds);
            ok &= sSearch.isEmpty();
            DspCmdWithParamsRaw lcmd;
            if (ok) {
                if (dspcmd->m_cmdExtraCheckFunction)
                    ok = dspcmd->m_cmdExtraCheckFunction(paramNames, threeI16Params, m_varResolver);
                if (ok) {
                    lcmd = DspCmdWithParamsRaw( dspcmd->CmdCode, (ushort)threeI16Params[0], (ushort)threeI16Params[1], (ushort)threeI16Params[2]);
                    if (dspcmd->modify)
                        lcmd.w[1] = (lcmd.w[1] & 0xFFFF) | (m_dspInterruptId << 16);
                }
            }
            return lcmd;
        }
        case CMD1i32:
        {
            long par;
            sSearch = CmdParser.GetKeyword(&cmds);
            ok = ( (par = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1);
            rawCollector->addCmd1Param(dspcmd, par);
            sSearch = CmdParser.GetKeyword(&cmds);
            ok &= sSearch.isEmpty();
            DspCmdWithParamsRaw lcmd;
            if (ok)
                lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode,(ulong)par);
            return lcmd;
        }
        case CMD1i161fi32:
        {
            short par1;
            long par2 = 0;
            sSearch = CmdParser.GetKeyword(&cmds);
            ok = ( (par1 = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1); // -1 ist fehlerbedingung
            DspCmdWithParamsRaw lcmd;
            if (!(ok))
                return lcmd; // wenn fehler -> fertig
            sSearch = CmdParser.GetKeyword(&cmds);
            par2 = sSearch.toLong(&ok); // test auf integer
            if (!ok)
                par2 = sSearch.toLong(&ok, 16); // test auf hex
            if (!ok)  {
                float tf = sSearch.toFloat(&ok);
                long* pl = (long*) &tf;
                par2= *pl;
            }
            rawCollector->addCmd2Params(dspcmd, par1, par2); // this needs love
            sSearch = CmdParser.GetKeyword(&cmds);
            ok &= sSearch.isEmpty();
            if (ok)
                lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode,(ushort)par1,(ulong)par2);
            return lcmd;
        }
        }
    }
    ok = false;
    return DspCmdWithParamsRaw();
}

bool DspCmdCompiler::compileCmds(const QString &cmdsSemicolonSeparated,
                                 QList<DspCmdWithParamsRaw> &genCmdList,
                                 QString &err,
                                 ulong userMemOffset,
                                 ulong globalstartadr,
                                 AbstractDspCompilerRawCollectorPtr rawCollector)
{
    bool ok = true;
    genCmdList.clear();
    const QStringList cmds = cmdsSemicolonSeparated.split(';', Qt::SkipEmptyParts);
    for(const QString &cmd : cmds) {
        genCmdList.append(compileOneCmdLineAligned(cmd, ok, userMemOffset, globalstartadr, rawCollector));
        if(!ok) {
            err = cmd;
            break;
        }
    }
    return ok;
}

bool DspCmdCompiler::syntaxCheck(const QString &dspCmdLine)
{
    int openPos = dspCmdLine.indexOf('(');
    int closePos = dspCmdLine.indexOf(')');
    bool ok = openPos > 0 && closePos>openPos;
    if(!ok)
        qCritical("Invalid command syntax: '%s'", qPrintable(dspCmdLine));
    return ok;
}
