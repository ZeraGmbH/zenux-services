#include "hkingroupresourceandinterface.h"
#include "zscpi_response_definitions.h"
#include "notzeronumgen.h"

enum Commands
{
    cmdVersion,
    cmdChannelCat
};

HkInGroupResourceAndInterface::HkInGroupResourceAndInterface(std::shared_ptr<cSCPI> scpiInterface,
                                                             HkInSettings *settings) :
    cResource(scpiInterface)
{
    QList<HkInSettings::ChannelSettings*> channelSettings;
    channelSettings = settings->getChannelSettings();
    // we have 1 hand key input channel
    HkInChannelInterface* pChannel = new HkInChannelInterface(m_scpiInterface, "Hand key input", 0, channelSettings.at(0));
    m_ChannelList.append(pChannel);
}

HkInGroupResourceAndInterface::~HkInGroupResourceAndInterface()
{
    for(auto channel : qAsConst(m_ChannelList))
        delete channel;
}

void HkInGroupResourceAndInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1HKEY").arg(leadingNodes),"VERSION",SCPI::isQuery, m_scpiInterface, cmdVersion);
    addDelegate(QString("%1HKEY:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_scpiInterface, cmdChannelCat);
    for(auto channel : qAsConst(m_ChannelList)) {
        connect(channel, &ScpiConnection::sigNotifySubcriber, this, &ScpiConnection::sigNotifySubcriber);
        connect(channel, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        channel->initSCPIConnection(QString("%1HKEY").arg(leadingNodes));
    }
}

void HkInGroupResourceAndInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    for(auto channel : qAsConst(m_ChannelList))
        register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("HKEY;%1;1;%2;%3;").arg(channel->getName(), channel->getDescription()).arg(port));
}

void HkInGroupResourceAndInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    switch (cmdCode)
    {
    case cmdVersion:
        protoCmd->m_sOutput = readVersion(protoCmd->m_sInput);
        break;
    case cmdChannelCat:
        protoCmd->m_sOutput = readChannelCatalog(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString HkInGroupResourceAndInterface::readVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return Version;
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString HkInGroupResourceAndInterface::readChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        int i;
        QString s;
        for (i = 0; i < m_ChannelList.count()-1; i++ )
            s += m_ChannelList.at(i)->getName() + ";";
        s += m_ChannelList.at(i)->getName();
        return s;
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}
