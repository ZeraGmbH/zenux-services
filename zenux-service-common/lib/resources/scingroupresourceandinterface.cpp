#include "scingroupresourceandinterface.h"
#include "notzeronumgen.h"

enum Commands
{
    cmdVersion,
    cmdChannelCat
};

ScInGroupResourceAndInterface::ScInGroupResourceAndInterface(cSCPI *scpiInterface, ScInSettings *settings) :
    cResource(scpiInterface)
{
    QList<ScInSettings::ChannelSettings*> channelSettings;
    channelSettings = settings->getChannelSettings();
    // we have 1 scanning head input channel
    ScInChannelInterface* pChannel;
    pChannel = new ScInChannelInterface(m_pSCPIInterface, "Scanning head input", 0, channelSettings.at(0) );
    m_ChannelList.append(pChannel);
}

ScInGroupResourceAndInterface::~ScInGroupResourceAndInterface()
{
    for(auto channel : qAsConst(m_ChannelList))
        delete channel;
}

void ScInGroupResourceAndInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(new cSCPIDelegate(QString("%1SCHEAD").arg(leadingNodes),"VERSION",SCPI::isQuery, m_pSCPIInterface, cmdVersion));
    addDelegate(new cSCPIDelegate(QString("%1SCHEAD:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, cmdChannelCat));
    for(auto channel : qAsConst(m_ChannelList)) {
        connect(channel, &ScpiConnection::strNotifier, this, &ScpiConnection::strNotifier);
        connect(channel, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        channel->initSCPIConnection(QString("%1SCHEAD").arg(leadingNodes));
    }
}

void ScInGroupResourceAndInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    for(auto channel : qAsConst(m_ChannelList))
        register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SCHEAD;%1;1;%2;%3;").arg(channel->getName()).arg(channel->getDescription()).arg(port));
}

void ScInGroupResourceAndInterface::unregisterResource(RMConnection *rmConnection)
{
    for(auto channel : qAsConst(m_ChannelList))
        unregister1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SCHEAD;%1;").arg(channel->getName()));
}

void ScInGroupResourceAndInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case cmdVersion:
        protoCmd->m_sOutput = m_ReadVersion(protoCmd->m_sInput);
        break;
    case cmdChannelCat:
        protoCmd->m_sOutput = m_ReadChannelCatalog(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString ScInGroupResourceAndInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return Version;
    else
        return SCPI::scpiAnswer[SCPI::nak];
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
        return SCPI::scpiAnswer[SCPI::nak];
}
