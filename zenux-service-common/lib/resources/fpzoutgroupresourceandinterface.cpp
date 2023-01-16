#include "fpzoutgroupresourceandinterface.h"
#include "scpiconnection.h"
#include "resource.h"
#include "fpzoutchannelinterface.h"
#include "protonetcommand.h"
#include "sourcesettings.h"
#include "notzeronumgen.h"

FpzOutGroupResourceAndInterface::FpzOutGroupResourceAndInterface(cSCPI *scpiInterface, cSourceSettings *settings) :
    cResource(scpiInterface)
{
    QList<SourceSystem::cChannelSettings*> channelSettings;
    channelSettings = settings->getChannelSettings();
    // we have 4 frequency output channels
    FpzOutChannelInterface* pChannel;
    pChannel = new FpzOutChannelInterface(m_pSCPIInterface, "Reference frequency output 0..1MHz", 0, channelSettings.at(0) );
    m_ChannelList.append(pChannel);
    pChannel = new FpzOutChannelInterface(m_pSCPIInterface, "Reference frequency output 0..1MHz", 1, channelSettings.at(1) );
    m_ChannelList.append(pChannel);
    pChannel = new FpzOutChannelInterface(m_pSCPIInterface, "Reference frequency output 0..1MHz", 2, channelSettings.at(2) );
    m_ChannelList.append(pChannel);
    pChannel = new FpzOutChannelInterface(m_pSCPIInterface, "Reference frequency output 0..1MHz", 3, channelSettings.at(3) );
    m_ChannelList.append(pChannel);
}

FpzOutGroupResourceAndInterface::~FpzOutGroupResourceAndInterface()
{
    for(auto channel : qAsConst(m_ChannelList))
        delete channel;
}

void FpzOutGroupResourceAndInterface::initSCPIConnection(QString leadingNodes)
{
    if (leadingNodes != "")
        leadingNodes += ":";
    cSCPIDelegate* delegate;
    delegate = new cSCPIDelegate(QString("%1SOURCE").arg(leadingNodes),"VERSION",SCPI::isQuery, m_pSCPIInterface, cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &FpzOutGroupResourceAndInterface::executeCommand);
    delegate = new cSCPIDelegate(QString("%1SOURCE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &FpzOutGroupResourceAndInterface::executeCommand);
    for(auto channel : qAsConst(m_ChannelList)) {
        connect(channel, &ScpiConnection::strNotifier, this, &ScpiConnection::strNotifier);
        connect(channel, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        channel->initSCPIConnection(QString("%1SOURCE").arg(leadingNodes));
    }
}

void FpzOutGroupResourceAndInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    for(auto channel : qAsConst(m_ChannelList))
        register1Resource(rmConnection,
                          NotZeroNumGen::getMsgNr(),
                          QString("SOURCE;%1;1;%2;%3;").arg(channel->getName()).arg(channel->getDescription()).arg(port));
}

void FpzOutGroupResourceAndInterface::unregisterResource(RMConnection *rmConnection)
{
    for(auto channel : qAsConst(m_ChannelList))
        unregister1Resource(rmConnection,
                            NotZeroNumGen::getMsgNr(),
                            QString("SOURCE;%1;").arg(channel->getName()));
}

void FpzOutGroupResourceAndInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
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

QString FpzOutGroupResourceAndInterface::readVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return Version;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString FpzOutGroupResourceAndInterface::readSourceChannelCatalog(QString &sInput)
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
