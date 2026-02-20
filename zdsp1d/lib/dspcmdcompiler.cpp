#include "dspcmdcompiler.h"

DspCmdCompiler::DspCmdCompiler(DspVarResolver *varResolver, int dspInterruptId) :
    m_varResolver(varResolver),
    m_dspInterruptId(dspInterruptId)
{
}

DspCmdWithParamsRaw DspCmdCompiler::compileOneCmdLineZeroAligned(const QString &cmdLine, bool *ok)
{
    return compileOneCmdLineAligned(cmdLine, ok, 0, 0);
}

DspCmdWithParamsRaw DspCmdCompiler::compileOneCmdLineAligned(const QString &cmdLine,
                                                             bool *ok,
                                                             ulong userMemOffset,
                                                             ulong globalstartadr,
                                                             DspCompilerRawCollector *rawCollector)
{
    cParse CmdParser;
    CmdParser.SetDelimiter("(,)"); // setze die trennzeichen für den parser
    CmdParser.SetWhiteSpace(" (,)");
    if(!syntaxCheck(cmdLine)) {
        *ok = false;
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
            if(rawCollector)
                rawCollector->addCmd(dspcmd);
            sSearch = CmdParser.GetKeyword(&cmds);
            *ok = sSearch.isEmpty(); // hier darf nichts stehen
            DspCmdWithParamsRaw lcmd;
            if (*ok)
                lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode);
            return lcmd;
        }
        case CMD1i16: // kommandowort, ein parameter
        {
            short par;
            bool t = true;
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= ( (par = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1); // -1 ist fehlerbedingung
            if(rawCollector)
                rawCollector->addCmd1Param(dspcmd, par);
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            DspCmdWithParamsRaw lcmd;
            if (t)
                lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode,(ushort)par);
            *ok = t;
            return lcmd;
        }
        case CMD2i16:
        {
            short twoI16Params[2];
            QStringList paramNames;
            bool t = true;
            for (int i=0; i<2; i++) {
                sSearch = CmdParser.GetKeyword(&cmds);
                paramNames.append(sSearch);
                t &= ( (twoI16Params[i] = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1);
            }
            if(rawCollector)
                rawCollector->addCmd2Params(dspcmd, twoI16Params[0], twoI16Params[1]);
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            DspCmdWithParamsRaw lcmd;
            if (t) {
                if (dspcmd->m_cmdExtraCheckFunction)
                    t = dspcmd->m_cmdExtraCheckFunction(paramNames, twoI16Params, m_varResolver);
                if (t) {
                    lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode, (ushort)twoI16Params[0], (ushort)twoI16Params[1]);
                    if (dspcmd->modify)
                        lcmd.w[1] = (lcmd.w[1] & 0xFFFF) | (m_dspInterruptId << 16);
                }
            }
            *ok = t;
            return lcmd;
        }
        case CMD3i16:
        {
            short par[3];
            bool t = true;
            for (int i=0; i<3; i++) {
                sSearch = CmdParser.GetKeyword(&cmds);
                t &= ( (par[i] = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1);
            }
            if(rawCollector)
                rawCollector->addCmd3Params(dspcmd, par[0], par[1], par[2]);
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            DspCmdWithParamsRaw lcmd;
            if (t) {
                lcmd = DspCmdWithParamsRaw( dspcmd->CmdCode, (ushort)par[0], (ushort)par[1], (ushort)par[2]);
                if (dspcmd->modify) lcmd.w[1] = (lcmd.w[1] & 0xFFFF) | (m_dspInterruptId << 16);
            }
            *ok = t;
            return lcmd;
        }
        case CMD1i32:
        {
            long par;
            sSearch = CmdParser.GetKeyword(&cmds);
            bool t = ( (par = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1);
            if(rawCollector)
                rawCollector->addCmd1Param(dspcmd, par);
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            DspCmdWithParamsRaw lcmd;
            if (t)
                lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode,(ulong)par);
            *ok = t;
            return lcmd;
        }
        case CMD1i161fi32:
        {
            short par1;
            long par2 = 0;
            sSearch = CmdParser.GetKeyword(&cmds);
            *ok = ( (par1 = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1); // -1 ist fehlerbedingung
            DspCmdWithParamsRaw lcmd;
            if (!(*ok))
                return lcmd; // wenn fehler -> fertig
            sSearch = CmdParser.GetKeyword(&cmds);
            bool t;
            par2 = sSearch.toLong(&t); // test auf integer
            if (!t)
                par2 = sSearch.toLong(&t,16); // test auf hex
            if (!t)  {
                float tf = sSearch.toFloat(&t);
                long* pl = (long*) &tf;
                par2= *pl;
            }
            if(rawCollector)
                rawCollector->addCmd2Params(dspcmd, par1, par2); // this needs love
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            if (t)
                lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode,(ushort)par1,(ulong)par2);
            *ok = t;
            return lcmd;
        }
        }
    }
    *ok = false;
    return DspCmdWithParamsRaw();
}

bool DspCmdCompiler::compileCmds(const QString &cmdsSemicolonSeparated,
                                 QList<DspCmdWithParamsRaw> &genCmdList,
                                 QString &err,
                                 ulong userMemOffset,
                                 ulong globalstartadr,
                                 DspCompilerRawCollector *rawCollector)
{
    bool ok = true;
    genCmdList.clear();
    const QStringList cmds = cmdsSemicolonSeparated.split(';', Qt::SkipEmptyParts);
    for(const QString &cmd : cmds) {
        genCmdList.append(compileOneCmdLineAligned(cmd, &ok, userMemOffset, globalstartadr, rawCollector));
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
