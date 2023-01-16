#include "sourceinterface.h"
#include "scpiconnection.h"
#include "resource.h"
#include "mt310s2d.h"
#include "fpzchannel.h"
#include "protonetcommand.h"
#include "sourcesettings.h"
#include "notzeronumgen.h"
#include <scpi.h>

cSourceInterface::cSourceInterface(cMT310S2dServer *server) :
    cResource(server->getSCPIInterface()),
    m_pMyServer(server)
{
    QList<SourceSystem::cChannelSettings*> channelSettings;
    channelSettings = m_pMyServer->m_pSourceSettings->getChannelSettings();

    // we have 4 frequency output channels
    cFPZChannel* pChannel;
    pChannel = new cFPZChannel(m_pSCPIInterface, "Reference frequency output 0..1MHz", 0, channelSettings.at(0) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZChannel(m_pSCPIInterface, "Reference frequency output 0..1MHz", 1, channelSettings.at(1) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZChannel(m_pSCPIInterface, "Reference frequency output 0..1MHz", 2, channelSettings.at(2) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZChannel(m_pSCPIInterface, "Reference frequency output 0..1MHz", 3, channelSettings.at(3) );
    m_ChannelList.append(pChannel);
    m_sVersion = SourceSystem::Version;
}

cSourceInterface::~cSourceInterface()
{
    for(auto channel : qAsConst(m_ChannelList)) {
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
    connect(delegate, &cSCPIDelegate::execute, this, &cSourceInterface::executeCommand);
    delegate = new cSCPIDelegate(QString("%1SOURCE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SourceSystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &cSourceInterface::executeCommand);
    for(auto channel : qAsConst(m_ChannelList)) {
        connect(channel, &ScpiConnection::strNotifier, this, &ScpiConnection::strNotifier);
        connect(channel, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        channel->initSCPIConnection(QString("%1SOURCE").arg(leadingNodes));
    }
}

void cSourceInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    for(auto channel : qAsConst(m_ChannelList)) {
        register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SOURCE;%1;1;%2;%3;").arg(channel->getName()).arg(channel->getDescription()).arg(port));
    }
}

void cSourceInterface::unregisterResource(RMConnection *rmConnection)
{
    for(auto channel : qAsConst(m_ChannelList)) {
        unregister1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SOURCE;%1;").arg(channel->getName()));
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
