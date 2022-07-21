#include "sourceinterface.h"
#include <scpiconnection.h>
#include "resource.h"
#include "mt310s2d.h"
#include "fpzchannel.h"
#include <protonetcommand.h>
#include "settings/sourcesettings.h"
#include <scpi.h>

cSourceInterface::cSourceInterface(cMT310S2dServer *server) :
    cResource(server->getSCPIInterface()),
    m_pMyServer(server)
{
    QList<SourceSystem::cChannelSettings*> channelSettings;
    channelSettings = m_pMyServer->m_pSourceSettings->getChannelSettings();

    // we have 4 frequency output channels
    cFPZChannel* pChannel;
    pChannel = new cFPZChannel(m_pMyServer->getSCPIInterface(), "Reference frequency output 0..1MHz", 0, channelSettings.at(0) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZChannel(m_pMyServer->getSCPIInterface(), "Reference frequency output 0..1MHz", 1, channelSettings.at(1) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZChannel(m_pMyServer->getSCPIInterface(), "Reference frequency output 0..1MHz", 2, channelSettings.at(2) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZChannel(m_pMyServer->getSCPIInterface(), "Reference frequency output 0..1MHz", 3, channelSettings.at(3) );
    m_ChannelList.append(pChannel);

    m_sVersion = SourceSystem::Version;
}

cSourceInterface::~cSourceInterface()
{
    for(auto channel : m_ChannelList) {
        delete channel;
    }
}

void cSourceInterface::initSCPIConnection(QString leadingNodes)
{
    if (leadingNodes != "")
        leadingNodes += ":";
    cSCPIDelegate* delegate;
    delegate = new cSCPIDelegate(QString("%1SOURCE").arg(leadingNodes),"VERSION",SCPI::isQuery, m_pSCPIInterface, SourceSystem::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SOURCE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SourceSystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    for(auto channel : m_ChannelList) {
        connect(channel, &cSCPIConnection::strNotifier, this, &cSCPIConnection::strNotifier);
        connect(channel, SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SIGNAL(cmdExecutionDone(cProtonetCommand*)));
        channel->initSCPIConnection(QString("%1SOURCE").arg(leadingNodes));
    }
}

void cSourceInterface::registerResource(cRMConnection *rmConnection, quint16 port)
{
    for(auto channel : m_ChannelList) {
        register1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SOURCE;%1;1;%2;%3;").arg(channel->getName()).arg(channel->getDescription()).arg(port));
    }
}

void cSourceInterface::unregisterResource(cRMConnection *rmConnection)
{
    for(auto channel : m_ChannelList) {
        unregister1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SOURCE;%1;").arg(channel->getName()));
    }
}

void cSourceInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SourceSystem::cmdVersion:
        protoCmd->m_sOutput = m_ReadVersion(protoCmd->m_sInput);
        break;
    case SourceSystem::cmdChannelCat:
        protoCmd->m_sOutput = m_ReadSourceChannelCatalog(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString cSourceInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sVersion;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString cSourceInterface::m_ReadSourceChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
    {
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




