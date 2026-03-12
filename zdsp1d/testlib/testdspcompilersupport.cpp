#include "testdspcompilersupport.h"

QStringList TestDspCompilerSupport::m_rawDspCommandsAllCyclic;
QStringList TestDspCompilerSupport::m_rawDspCommandsAllInterrupt;

void TestDspCompilerSupport::startClientArea(int entityId, QString additionalInfo, MemType memType)
{
    m_currMemType = memType;
    QString message = QString("-- Start entity ID %1 / %2 --").arg(entityId).arg(additionalInfo);
    if (memType == CYCLIC)
        m_rawDspCommandsAllCyclic.append(message);
    else
        m_rawDspCommandsAllInterrupt.append(message);
}

void TestDspCompilerSupport::clearTotalCmdLists()
{
    m_rawDspCommandsAllCyclic.clear();
    m_rawDspCommandsAllInterrupt.clear();
}

const QStringList &TestDspCompilerSupport::getRawDspCommandsAllVerbose(MemType memType)
{
    if (memType == CYCLIC)
        return m_rawDspCommandsAllCyclic;
    return m_rawDspCommandsAllInterrupt;
}

int TestDspCompilerSupport::getRawDspCommandsCount(MemType memType)
{
    const QStringList &cmdsVerbose = getRawDspCommandsAllVerbose(memType);
    int rawCount = 0;
    for (const QString &cmd : cmdsVerbose) {
        if (!cmd.startsWith("--"))
            rawCount++;
    }
    return rawCount;
}

int TestDspCompilerSupport::getRawDspIntTriggerCount(MemType memType)
{
    const QStringList &rawCommands = memType == CYCLIC ? m_rawDspCommandsAllCyclic : m_rawDspCommandsAllInterrupt;
    int interruptTriggeCount = 0;
    for (const QString &cmd : rawCommands)
        if (cmd.contains("DSPINTTRIGGER"))
            interruptTriggeCount++;
    return interruptTriggeCount;
}

bool TestDspCompilerSupport::addCmdToRaw(const QString &dspCmdLine,
                                         const QStringList &paramNames,
                                         const short i16Params[],
                                         const DspCmdDecodingDetails *dspcmd,
                                         DspVarResolver *varResolver)
{
    QString raw = QString(dspcmd->Name) + "(";
    for (int paramNo=0; paramNo<paramNames.count(); paramNo++) {
        ulong par = i16Params[paramNo];
        raw += DspVarInServer::toHex(par);
        if (paramNo<paramNames.count()-1)
            raw += ",";
    }
    raw += ")";
    appendCmdRaw(raw);

    return allChecks(dspCmdLine, paramNames, i16Params, dspcmd, varResolver);
}

bool TestDspCompilerSupport::addCmdToRaw1Param(const QString &dspCmdLine,
                                               ulong par,
                                               const DspCmdDecodingDetails *dspcmd)
{
    // This is just about fixed USERMEMOFFSET / DSPMEMOFFSET
    QString hexPar = DspVarInServer::toHex(par);
    appendCmdRaw(QString("%1(%2)").arg(QString(dspcmd->Name), hexPar));
    return syntaxCheck(dspCmdLine);
}

bool TestDspCompilerSupport::addCmdToRaw2Params(const QString &dspCmdLine, const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2)
{
    QString hexPar1 = DspVarInServer::toHex(par1);
    QString hexPar2 = DspVarInServer::toHex(par2);
    appendCmdRaw(QString("%1(%2,%3)").arg(QString(dspcmd->Name), hexPar1, hexPar2));
    return syntaxCheck(dspCmdLine);
}

const QStringList &TestDspCompilerSupport::getRawDspCommands(MemType memType) const
{
    if (memType == CYCLIC)
        return m_rawDspCommandsClientCyclic;
    return m_rawDspCommandsClientInterrupt;
}

void TestDspCompilerSupport::appendCmdRaw(const QString &cmdRaw)
{
    if (m_currMemType == CYCLIC) {
        m_rawDspCommandsClientCyclic.append(cmdRaw);
        m_rawDspCommandsAllCyclic.append(cmdRaw);
    }
    else {
        m_rawDspCommandsClientInterrupt.append(cmdRaw);
        m_rawDspCommandsAllInterrupt.append(cmdRaw);
    }
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
