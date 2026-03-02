#include "testdspcompilersupport.h"

bool TestDspCompilerSupport::addCmdToRaw(const QString &dspCmdLine,
                                         const QStringList &paramNames,
                                         const short i16Params[],
                                         const DspCmdDecodingDetails *dspcmd,
                                         DspVarResolver *varResolver)
{
    QString raw = QString(dspcmd->Name) + "(";
    for (int paramNo=0; paramNo<paramNames.count(); paramNo++) {
        ulong par = i16Params[paramNo];
        raw += DspVarServer::toHex(par);
        if (paramNo<paramNames.count()-1)
            raw += ",";
    }
    raw += ")";
    m_rawDspCommands.append(raw);

    return allChecks(dspCmdLine, paramNames, i16Params, dspcmd, varResolver);
}

bool TestDspCompilerSupport::addCmdToRaw1Param(const QString &dspCmdLine,
                                               ulong par,
                                               const DspCmdDecodingDetails *dspcmd)
{
    // This is just about fixed USERMEMOFFSET / DSPMEMOFFSET
    QString hexPar = DspVarServer::toHex(par);
    m_rawDspCommands.append(QString("%1(%2)").arg(QString(dspcmd->Name), hexPar));
    return syntaxCheck(dspCmdLine);
}

bool TestDspCompilerSupport::addCmdToRaw2Params(const QString &dspCmdLine, const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2)
{
    QString hexPar1 = DspVarServer::toHex(par1);
    QString hexPar2 = DspVarServer::toHex(par2);
    m_rawDspCommands.append(QString("%1(%2,%3)").arg(QString(dspcmd->Name), hexPar1, hexPar2));
    return syntaxCheck(dspCmdLine);
}

const QStringList &TestDspCompilerSupport::getRawDspCommands() const
{
    return m_rawDspCommands;
}

bool TestDspCompilerSupport::allChecks(const QString &dspCmdLine, const QStringList &paramNames,
                                       const short i16Params[],
                                       const DspCmdDecodingDetails *dspcmd,
                                       DspVarResolver *varResolver)
{
    bool ok = syntaxCheck(dspCmdLine);
    if (dspcmd->m_cmdExtraCheckFunction)
        ok &= dspcmd->m_cmdExtraCheckFunction(paramNames, i16Params, varResolver);
    return ok;

}

bool TestDspCompilerSupport::syntaxCheck(const QString &dspCmdLine)
{
    int openPos = dspCmdLine.indexOf('(');
    int closePos = dspCmdLine.indexOf(')');
    bool ok = openPos > 0 && closePos>openPos;
    if(!ok)
        qCritical("Invalid command syntax: '%s'", qPrintable(dspCmdLine));
    return ok;
}
