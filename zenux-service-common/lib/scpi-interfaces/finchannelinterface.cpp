#include "finchannelinterface.h"

enum Commands
{
    cmdAlias,
    cmdStatus
};

FInChannelInterface::FInChannelInterface(cSCPI *scpiInterface, QString description, quint8 nr, FInSettings::ChannelSettings *cSettings) :
    ScpiConnection(scpiInterface),
    m_sDescription(description)
{
    m_sName = QString("fi%1").arg(nr);
    m_sAlias = cSettings->m_sAlias;
    m_bAvail = cSettings->avail;
}

void FInChannelInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ALIAS", SCPI::isQuery, m_pSCPIInterface, cmdAlias));
    addDelegate(new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, m_pSCPIInterface, cmdStatus));
}

void FInChannelInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case cmdAlias:
        protoCmd->m_sOutput = readAlias(protoCmd->m_sInput);
        break;
    case cmdStatus:
        protoCmd->m_sOutput = readChannelStatus(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString &FInChannelInterface::getName()
{
    return m_sName;
}

QString &FInChannelInterface::getAlias()
{
    return m_sAlias;
}

QString &FInChannelInterface::getDescription()
{
    return m_sDescription;
}

bool FInChannelInterface::isAvail()
{
    return m_bAvail;
}

QString FInChannelInterface::readAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sAlias;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString FInChannelInterface::readChannelStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        quint32 r = ((m_bAvail) ? 0 : 1 << 31);
        return QString("%1").arg(r);
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}
