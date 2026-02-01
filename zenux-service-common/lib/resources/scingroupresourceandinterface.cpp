#include "scingroupresourceandinterface.h"
#include "zscpi_response_definitions.h"
#include "notzeronumgen.h"

enum Commands
{
    cmdVersion,
    cmdChannelCat
};

ScInGroupResourceAndInterface::ScInGroupResourceAndInterface(std::shared_ptr<cSCPI> scpiInterface,
                                                             ScInSettings *settings) :
    cResource(scpiInterface)
{
    QList<ScInSettings::ChannelSettings*> channelSettings;
    channelSettings = settings->getChannelSettings();
    // we have 1 scanning head input channel
    ScInChannelInterface* pChannel;
    pChannel = new ScInChannelInterface(m_scpiInterface, "Scanning head input", 0, channelSettings.at(0) );
    m_ChannelList.append(pChannel);
}

ScInGroupResourceAndInterface::~ScInGroupResourceAndInterface()
{
    for(auto channel : qAsConst(m_ChannelList))
        delete channel;
}

void ScInGroupResourceAndInterface::initSCPIConnection(QString leadingNodes)
{
    const QString adjLeadNodes = appendTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SCHEAD").arg(adjLeadNodes),"VERSION",SCPI::isQuery, m_scpiInterface, cmdVersion);
    addDelegate(QString("%1SCHEAD:CHANNEL").arg(adjLeadNodes),"CATALOG", SCPI::isQuery, m_scpiInterface, cmdChannelCat);
    for(auto channel : qAsConst(m_ChannelList)) {
        connect(channel, &ScpiConnection::sigNotifySubcriber, this, &ScpiConnection::sigNotifySubcriber);
        connect(channel, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        channel->initSCPIConnection(QString("%1SCHEAD").arg(adjLeadNodes));
    }
}

void ScInGroupResourceAndInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    for(auto channel : qAsConst(m_ChannelList))
        register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SCHEAD;%1;1;%2;%3;").arg(channel->getName(), channel->getDescription()).arg(port));
}

void ScInGroupResourceAndInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    switch (cmdCode)
    {
    case cmdVersion:
        protoCmd->m_sOutput = scpiReadVersion(protoCmd->m_sInput);
        break;
    case cmdChannelCat:
        protoCmd->m_sOutput = m_ReadChannelCatalog(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString ScInGroupResourceAndInterface::scpiReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return Version;
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString ScInGroupResourceAndInterface::m_ReadChannelCatalog(QString &sInput)
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
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}
