#include "fingroupresourceandinterface.h"
#include "zscpi_response_definitions.h"
#include "notzeronumgen.h"

enum Commands
{
    cmdVersion,
    cmdChannelCat
};

FInGroupResourceAndInterface::FInGroupResourceAndInterface(std::shared_ptr<cSCPI> scpiInterface,
                                                           FInSettings *settings) :
    cResource(scpiInterface)
{
    QList<FInSettings::ChannelSettings*> channelSettings;
    channelSettings = settings->getChannelSettings();

    // we have 4 frequency input channels
    FInChannelInterface* pChannel;
    pChannel = new FInChannelInterface(m_scpiInterface, "Frequency input 0..1MHz", 0, channelSettings.at(0) );
    m_ChannelList.append(pChannel);
    pChannel = new FInChannelInterface(m_scpiInterface, "Frequency output 0..1MHz", 1, channelSettings.at(1) );
    m_ChannelList.append(pChannel);
    pChannel = new FInChannelInterface(m_scpiInterface, "Frequency output 0..1MHz", 2,  channelSettings.at(2) );
    m_ChannelList.append(pChannel);
    pChannel = new FInChannelInterface(m_scpiInterface, "Frequency output 0..1MHz", 3, channelSettings.at(3) );
    m_ChannelList.append(pChannel);
}

FInGroupResourceAndInterface::~FInGroupResourceAndInterface()
{
    for(auto channel : qAsConst(m_ChannelList))
        delete channel;
}

void FInGroupResourceAndInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1FRQINPUT").arg(leadingNodes),"VERSION",SCPI::isQuery, m_scpiInterface, cmdVersion);
    addDelegate(QString("%1FRQINPUT:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_scpiInterface, cmdChannelCat);
    for (auto channel : qAsConst(m_ChannelList)) {
        connect(channel, &ScpiConnection::sigNotifySubcriber, this, &ScpiConnection::sigNotifySubcriber);
        connect(channel, &FInChannelInterface::cmdExecutionDone, this, &FInGroupResourceAndInterface::cmdExecutionDone);
        channel->initSCPIConnection(QString("%1FRQINPUT").arg(leadingNodes));
    }
}

void FInGroupResourceAndInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    for(auto channel : qAsConst(m_ChannelList))
        register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("FRQINPUT;%1;1;%2;%3;").arg(channel->getName(), channel->getDescription()).arg(port));
}

void FInGroupResourceAndInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
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

QString FInGroupResourceAndInterface::readVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return Version;
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString FInGroupResourceAndInterface::readChannelCatalog(QString &sInput)
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
