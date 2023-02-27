#include "foutgroupresourceandinterface.h"
#include "notzeronumgen.h"

enum Commands
{
    cmdVersion,
    cmdChannelCat
};

FOutGroupResourceAndInterface::FOutGroupResourceAndInterface(cSCPI *scpiInterface, FOutSettings *settings) :
    cResource(scpiInterface)
{
    QList<FOutSettings::ChannelSettings*> channelSettings = settings->getChannelSettings();
    // we have 4 frequency output channels
    FOutChannelInterface* pChannel;
    pChannel = new FOutChannelInterface(m_pSCPIInterface, "Reference frequency output 0..1MHz", 0, channelSettings.at(0) );
    m_ChannelList.append(pChannel);
    pChannel = new FOutChannelInterface(m_pSCPIInterface, "Reference frequency output 0..1MHz", 1, channelSettings.at(1) );
    m_ChannelList.append(pChannel);
    pChannel = new FOutChannelInterface(m_pSCPIInterface, "Reference frequency output 0..1MHz", 2, channelSettings.at(2) );
    m_ChannelList.append(pChannel);
    pChannel = new FOutChannelInterface(m_pSCPIInterface, "Reference frequency output 0..1MHz", 3, channelSettings.at(3) );
    m_ChannelList.append(pChannel);
}

FOutGroupResourceAndInterface::~FOutGroupResourceAndInterface()
{
    for(auto channel : qAsConst(m_ChannelList))
        delete channel;
}

void FOutGroupResourceAndInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SOURCE").arg(leadingNodes),"VERSION",SCPI::isQuery, m_pSCPIInterface, cmdVersion);
    addDelegate(QString("%1SOURCE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, cmdChannelCat);
    for(auto channel : qAsConst(m_ChannelList)) {
        connect(channel, &ScpiConnection::sendNotification, this, &ScpiConnection::sendNotification);
        connect(channel, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        connect(this, &ScpiConnection::removingSubscribers, channel, &ScpiConnection::onRemoveSubscribers);
        channel->initSCPIConnection(QString("%1SOURCE").arg(leadingNodes));
    }
}

void FOutGroupResourceAndInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    for(auto channel : qAsConst(m_ChannelList))
        register1Resource(rmConnection,
                          NotZeroNumGen::getMsgNr(),
                          QString("SOURCE;%1;1;%2;%3;").arg(channel->getName()).arg(channel->getDescription()).arg(port));
}

void FOutGroupResourceAndInterface::unregisterResource(RMConnection *rmConnection)
{
    for(auto channel : qAsConst(m_ChannelList))
        unregister1Resource(rmConnection,
                            NotZeroNumGen::getMsgNr(),
                            QString("SOURCE;%1;").arg(channel->getName()));
}

void FOutGroupResourceAndInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
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
        return SCPI::scpiAnswer[SCPI::nak];
}

QString FOutGroupResourceAndInterface::readSourceChannelCatalog(QString &sInput)
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
    else
        return SCPI::scpiAnswer[SCPI::nak];
}
