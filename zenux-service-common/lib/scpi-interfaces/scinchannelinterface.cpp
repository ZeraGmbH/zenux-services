#include "scinchannelinterface.h"
#include "zscpi_response_definitions.h"

enum Commands
{
    cmdAlias,
    cmdStatus
};

ScInChannelInterface::ScInChannelInterface(std::shared_ptr<cSCPI> scpiinterface,
                                           QString description,
                                           quint8 nr,
                                           ScInSettings::ChannelSettings *cSettings) :
    ScpiConnection(scpiinterface),
    m_sDescription(description)
{
    m_sName = QString("sh%1").arg(nr);
    m_sAlias = cSettings->m_alias;
    m_bAvail = cSettings->m_avail;
}

void ScInChannelInterface::initSCPIConnection(QString leadingNodes)
{
    const QString adjLeadNodes = appendTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_sName),"ALIAS", SCPI::isQuery, m_scpiInterface, cmdAlias);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_sName),"STATUS", SCPI::isQuery, m_scpiInterface, cmdStatus);
}

void ScInChannelInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    switch (cmdCode)
    {
    case cmdAlias:
        protoCmd->m_sOutput = scpiReadAlias(protoCmd->m_sInput);
        break;
    case cmdStatus:
        protoCmd->m_sOutput = scpiReadChannelStatus(protoCmd->m_sInput);
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

QString ScInChannelInterface::scpiReadAlias(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return m_sAlias;
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString ScInChannelInterface::scpiReadChannelStatus(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        quint32 r = ((m_bAvail) ? 0 : 1 << 31);
        return QString("%1").arg(r);
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}
