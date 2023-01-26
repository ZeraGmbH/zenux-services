#include "hkinchannelinterface.h"

enum Commands
{
    cmdAlias,
    cmdStatus
};

HkInChannelInterface::HkInChannelInterface(cSCPI *scpiinterface, QString description, quint8 nr, HkInSettings::ChannelSettings *cSettings) :
    ScpiConnection(scpiinterface),
    m_sDescription(description)
{
    m_sName = QString("hk%1").arg(nr);
    m_sAlias = cSettings->m_sAlias;
    m_bAvail = cSettings->avail;
}

void HkInChannelInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ALIAS", SCPI::isQuery, m_pSCPIInterface, cmdAlias);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, m_pSCPIInterface, cmdStatus);
}

void HkInChannelInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
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

QString &HkInChannelInterface::getName()
{
    return m_sName;
}

QString &HkInChannelInterface::getAlias()
{
    return m_sAlias;
}

QString &HkInChannelInterface::getDescription()
{
    return m_sDescription;
}

bool HkInChannelInterface::isAvail()
{
    return m_bAvail;
}

QString HkInChannelInterface::readAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sAlias;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString HkInChannelInterface::readChannelStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        quint32 r = ((m_bAvail) ? 0 : 1 << 31);
        return QString("%1").arg(r);
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

