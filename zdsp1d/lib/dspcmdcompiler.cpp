#include "dspcmdcompiler.h"
#include "dspcompilersupport.h"

DspCmdCompiler::DspCmdCompiler(DspVarResolver *varResolver, int dspInterruptId) :
    m_varResolver(varResolver),
    m_dspInterruptId(dspInterruptId)
{
}

DspCmdWithParamsRaw DspCmdCompiler::compileOneCmdLineZeroAligned(const QString &cmdLine,
                                                                 bool &ok)
{
    return compileOneCmdLineAligned(cmdLine, ok, 0, 0, std::make_shared<DspCompilerSupport>());
}

DspCmdWithParamsRaw DspCmdCompiler::compileOneCmdLineAligned(const QString &cmdLine,
                                                             bool &ok,
                                                             ulong userMemOffset,
                                                             ulong globalstartadr,
                                                             AbstractDspCompilerSupportPtr compilerSupport)
{
    ok = true;
    cParse cmdParser;
    cmdParser.SetDelimiter("(,)"); // setze die trennzeichen für den parser
    cmdParser.SetWhiteSpace(" (,)");

    const QChar* charCmdLine = cmdLine.data();
    QString sSearch = cmdParser.GetKeyword(&charCmdLine); // das 1. keyword muss ein befehlscode sein
    DspCmdDecodingDetails *dspcmd = DspStaticData::findDspCmd(sSearch);
    if (dspcmd) {
        QStringList paramNames;
        short paramValues[3];

        switch (dspcmd->CmdClass) {
        case CMD: // command / no parameters
        {
            ok &= compilerSupport->addCmdToRaw(cmdLine, paramNames, paramValues, dspcmd, m_varResolver);
            ok &= areThereNoFurtherKeywords(cmdParser, charCmdLine);
            DspCmdWithParamsRaw lcmd;
            if (ok)
                lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode);
            return lcmd;
        }
        case CMD1i16: // command, one i16 parameter
        {
            sSearch = cmdParser.GetKeyword(&charCmdLine);
            paramNames.append(sSearch);
            ok &= ( (paramValues[0] = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1); // -1 ist fehlerbedingung
            ok &= compilerSupport->addCmdToRaw(cmdLine, paramNames, paramValues, dspcmd, m_varResolver);
            ok &= areThereNoFurtherKeywords(cmdParser, charCmdLine);
            DspCmdWithParamsRaw lcmd;
            if (ok)
                lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode,(ushort)paramValues[0]);
            return lcmd;
        }
        case CMD2i16: // command, two i16 parameters
        {
            for (int i=0; i<2; i++) {
                sSearch = cmdParser.GetKeyword(&charCmdLine);
                paramNames.append(sSearch);
                ok &= ( (paramValues[i] = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1);
            }
            ok &= compilerSupport->addCmdToRaw(cmdLine, paramNames, paramValues, dspcmd, m_varResolver);
            ok &= areThereNoFurtherKeywords(cmdParser, charCmdLine);
            DspCmdWithParamsRaw lcmd;
            if (ok) {
                lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode, (ushort)paramValues[0], (ushort)paramValues[1]);
                if (dspcmd->modify)
                    lcmd.w[1] = (lcmd.w[1] & 0xFFFF) | (m_dspInterruptId << 16);
            }
            return lcmd;
        }
        case CMD3i16: // command, three i16 parameters
        {
            for (int i=0; i<3; i++) {
                sSearch = cmdParser.GetKeyword(&charCmdLine);
                paramNames.append(sSearch);
                ok &= ( (paramValues[i] = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1);
            }
            ok &= compilerSupport->addCmdToRaw(cmdLine, paramNames, paramValues, dspcmd, m_varResolver);
            ok &= areThereNoFurtherKeywords(cmdParser, charCmdLine);
            DspCmdWithParamsRaw lcmd;
            if (ok) {
                lcmd = DspCmdWithParamsRaw( dspcmd->CmdCode, (ushort)paramValues[0], (ushort)paramValues[1], (ushort)paramValues[2]);
                if (dspcmd->modify)
                    lcmd.w[1] = (lcmd.w[1] & 0xFFFF) | (m_dspInterruptId << 16);
            }
            return lcmd;
        }
        case CMD1i32: // command, one i32 parameter (USERMEMOFFSET / DSPMEMOFFSET only)
        {
            long par;
            sSearch = cmdParser.GetKeyword(&charCmdLine);
            ok &= ( (par = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1);
            ok &= compilerSupport->addCmdToRaw1Param(cmdLine, par, dspcmd);
            ok &= areThereNoFurtherKeywords(cmdParser, charCmdLine);
            DspCmdWithParamsRaw lcmd;
            if (ok)
                lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode,(ulong)par);
            return lcmd;
        }
        case CMD1i161fi32:
        {
            short par1;
            long par2 = 0;
            sSearch = cmdParser.GetKeyword(&charCmdLine);
            ok &= ( (par1 = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1); // -1 ist fehlerbedingung
            DspCmdWithParamsRaw lcmd;
            if (!(ok))
                return lcmd; // wenn fehler -> fertig
            sSearch = cmdParser.GetKeyword(&charCmdLine);
            par2 = sSearch.toLong(&ok); // test auf integer
            if (!ok)
                par2 = sSearch.toLong(&ok, 16); // test auf hex
            if (!ok)  {
                float tf = sSearch.toFloat(&ok);
                long* pl = (long*) &tf;
                par2= *pl;
            }
            ok &= compilerSupport->addCmdToRaw2Params(cmdLine, dspcmd, par1, par2); // this needs love
            ok &= areThereNoFurtherKeywords(cmdParser, charCmdLine);
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
                                 AbstractDspCompilerSupportPtr compilerSupport)
{
    bool ok = true;
    genCmdList.clear();
    const QStringList cmds = cmdsSemicolonSeparated.split(';', Qt::SkipEmptyParts);
    for(const QString &cmd : cmds) {
        genCmdList.append(compileOneCmdLineAligned(cmd, ok, userMemOffset, globalstartadr, compilerSupport));
        if(!ok) {
            err = cmd;
            break;
        }
    }
    return ok;
}

bool DspCmdCompiler::areThereNoFurtherKeywords(cParse &cmdParser, const QChar *charCmdLine)
{
    return cmdParser.GetKeyword(&charCmdLine).isEmpty();
}
