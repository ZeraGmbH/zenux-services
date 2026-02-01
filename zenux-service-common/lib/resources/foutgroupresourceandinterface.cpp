#include "foutgroupresourceandinterface.h"
#include "zscpi_response_definitions.h"
#include "notzeronumgen.h"

enum Commands
{
    cmdVersion,
    cmdChannelCat
};

FOutGroupResourceAndInterface::FOutGroupResourceAndInterface(std::shared_ptr<cSCPI> scpiInterface,
                                                             FOutSettings *settings) :
    cResource(scpiInterface)
{
    QList<FOutSettings::ChannelSettings*> channelSettings = settings->getChannelSettings();
    // we have 4 frequency output channels
    FOutChannelInterface* pChannel;
    pChannel = new FOutChannelInterface(m_scpiInterface, "Reference frequency output 0..1MHz", 0, channelSettings.at(0) );
    m_ChannelList.append(pChannel);
    pChannel = new FOutChannelInterface(m_scpiInterface, "Reference frequency output 0..1MHz", 1, channelSettings.at(1) );
    m_ChannelList.append(pChannel);
    pChannel = new FOutChannelInterface(m_scpiInterface, "Reference frequency output 0..1MHz", 2, channelSettings.at(2) );
    m_ChannelList.append(pChannel);
    pChannel = new FOutChannelInterface(m_scpiInterface, "Reference frequency output 0..1MHz", 3, channelSettings.at(3) );
    m_ChannelList.append(pChannel);
}

FOutGroupResourceAndInterface::~FOutGroupResourceAndInterface()
{
    for(auto channel : qAsConst(m_ChannelList))
        delete channel;
}

void FOutGroupResourceAndInterface::initSCPIConnection(const QString &leadingNodes)
{
    const QString adjLeadNodes = appendTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SOURCE").arg(adjLeadNodes),"VERSION",SCPI::isQuery, m_scpiInterface, cmdVersion);
    addDelegate(QString("%1SOURCE:CHANNEL").arg(adjLeadNodes),"CATALOG", SCPI::isQuery, m_scpiInterface, cmdChannelCat);
    for(auto channel : qAsConst(m_ChannelList)) {
        connect(channel, &ScpiConnection::sigNotifySubcriber, this, &ScpiConnection::sigNotifySubcriber);
        connect(channel, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        connect(this, &ScpiConnection::removingSubscribers, channel, &ScpiConnection::onRemoveSubscribers);
        channel->initSCPIConnection(QString("%1SOURCE").arg(adjLeadNodes));
    }
}

void FOutGroupResourceAndInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    for(auto channel : qAsConst(m_ChannelList))
        register1Resource(rmConnection,
                          NotZeroNumGen::getMsgNr(),
                          QString("SOURCE;%1;1;%2;%3;").arg(channel->getName(), channel->getDescription()).arg(port));
}

void FOutGroupResourceAndInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    switch (cmdCode)
    {
    case cmdVersion:
        protoCmd->m_sOutput = readVersion(protoCmd->m_sInput);
        break;
    case cmdChannelCat:
        protoCmd->m_sOutput = readSourceChannelCatalog(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString FOutGroupResourceAndInterface::readVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return Version;
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString FOutGroupResourceAndInterface::readSourceChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        QStringList channelList;
        for(const auto &channel : qAsConst(m_ChannelList))
            channelList.append(channel->getName());
        return channelList.join(";");
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}
