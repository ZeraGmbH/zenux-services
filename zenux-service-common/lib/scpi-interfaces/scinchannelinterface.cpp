#include "scinchannelinterface.h"
#include "zscpi_response_definitions.h"

enum Commands
{
    cmdAlias,
    cmdStatus
};

ScInChannelInterface::ScInChannelInterface(cSCPI *scpiinterface, QString description, quint8 nr, ScInSettings::ChannelSettings *cSettings) :
    ScpiConnection(scpiinterface),
    m_sDescription(description)
{
    m_sName = QString("sh%1").arg(nr);
    m_sAlias = cSettings->m_alias;
    m_bAvail = cSettings->m_avail;
}

void ScInChannelInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName),"ALIAS", SCPI::isQuery, m_pSCPIInterface, cmdAlias);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName),"STATUS", SCPI::isQuery, m_pSCPIInterface, cmdStatus);
}

void ScInChannelInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case cmdAlias:
        protoCmd->m_sOutput = m_ReadAlias(protoCmd->m_sInput);
        break;
    case cmdStatus:
        protoCmd->m_sOutput = m_ReadChannelStatus(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString &ScInChannelInterface::getName()
{
    return m_sName;
}

QString &ScInChannelInterface::getAlias()
{
    return m_sAlias;
}

QString &ScInChannelInterface::getDescription()
{
    return m_sDescription;
}

bool ScInChannelInterface::isAvail()
{
    return m_bAvail;
}

QString ScInChannelInterface::m_ReadAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sAlias;
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString ScInChannelInterface::m_ReadChannelStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        quint32 r = ((m_bAvail) ? 0 : 1 << 31);
        return QString("%1").arg(r);
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}
