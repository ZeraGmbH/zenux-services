#include "hkinchannelinterface.h"
#include "zscpi_response_definitions.h"

enum Commands
{
    cmdAlias,
    cmdStatus
};

HkInChannelInterface::HkInChannelInterface(std::shared_ptr<cSCPI> scpiinterface,
                                           QString description,
                                           quint8 nr,
                                           HkInSettings::ChannelSettings *cSettings) :
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
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName),"ALIAS", SCPI::isQuery, m_scpiInterface, cmdAlias);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName),"STATUS", SCPI::isQuery, m_scpiInterface, cmdStatus);
}

void HkInChannelInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
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

const QString &HkInChannelInterface::getName()
{
    return m_sName;
}

const QString &HkInChannelInterface::getAlias()
{
    return m_sAlias;
}

const QString &HkInChannelInterface::getDescription()
{
    return m_sDescription;
}

bool HkInChannelInterface::isAvail()
{
    return m_bAvail;
}

QString HkInChannelInterface::scpiReadAlias(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return m_sAlias;
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString HkInChannelInterface::scpiReadChannelStatus(const QString& scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        quint32 r = ((m_bAvail) ? 0 : 1 << 31);
        return QString("%1").arg(r);
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

