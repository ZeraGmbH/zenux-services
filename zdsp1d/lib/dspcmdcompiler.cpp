#include "dspcmdcompiler.h"

DspCmdCompiler::DspCmdCompiler(DspVarResolver *varResolver, int socket) :
    m_varResolver(varResolver),
    m_socket(socket)
{
}

DspCmdWithParamsRaw DspCmdCompiler::compileOneCmdLineZeroAligned(const QString &cmdLine, bool *ok)
{
    return compileOneCmdLineAligned(cmdLine, ok, 0, 0);
}

DspCmdWithParamsRaw DspCmdCompiler::compileOneCmdLineAligned(const QString &cmdLine,
                                                             bool *ok,
                                                             ulong userMemOffset,
                                                             ulong globalstartadr)
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
    sDspCmd *dspcmd = findDspCmd(sSearch);
    if (dspcmd)
    { // bekannter befehlscode ?
        switch (dspcmd->CmdClass) {
        case CMD: // nur kommandowort, kein parameter
        {
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
            short par[2];
            bool t = true;
            for (int i=0; i<2; i++) {
                sSearch = CmdParser.GetKeyword(&cmds);
                t &= ( (par[i] = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1);
            }
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            DspCmdWithParamsRaw lcmd;
            if (t) {
                lcmd = DspCmdWithParamsRaw(dspcmd->CmdCode, (ushort)par[0], (ushort)par[1]);
                if (dspcmd->modify) lcmd.w[1] = (lcmd.w[1] & 0xFFFF) | (m_socket << 16);
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
            sSearch = CmdParser.GetKeyword(&cmds);
            t &= sSearch.isEmpty();
            DspCmdWithParamsRaw lcmd;
            if (t) {
                lcmd = DspCmdWithParamsRaw( dspcmd->CmdCode, (ushort)par[0], (ushort)par[1], (ushort)par[2]);
                if (dspcmd->modify) lcmd.w[1] = (lcmd.w[1] & 0xFFFF) | (m_socket << 16);
            }
            *ok = t;
            return lcmd;
        }
        case CMD1i32:
        {
            long par;
            sSearch = CmdParser.GetKeyword(&cmds);
            bool t = ( (par = m_varResolver->getVarOffset(sSearch, userMemOffset, globalstartadr)) > -1);
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

bool DspCmdCompiler::compileCmds(const QString &cmdsSemicolonSeparated, QList<DspCmdWithParamsRaw> &genCmdList, QString &err, ulong userMemOffset, ulong globalstartadr)
{
    bool ok = true;
    genCmdList.clear();
    for(int i = 0;;i++) {
        QString cs = cmdsSemicolonSeparated.section(';',i,i);
        if ( (cs.isEmpty()) || (cs==("Empty")) )
            break; // liste ist durch
        genCmdList.append(compileOneCmdLineAligned(cs, &ok, userMemOffset, globalstartadr));
        if(!ok) {
            err = cs;
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
