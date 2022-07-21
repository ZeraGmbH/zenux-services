#include "hkeyinterface.h"
#include "resource.h"
#include "mt310s2d.h"
#include "hkeychannel.h"
#include <protonetcommand.h>
#include "settings/hkeysettings.h"
#include <xmlsettings.h>
#include <scpi.h>

cHKeyInterface::cHKeyInterface(cMT310S2dServer *server) :
    cResource(server->getSCPIInterface()),
    m_pMyServer(server)
{
    QList<HKeySystem::cChannelSettings*> channelSettings;
    channelSettings = m_pMyServer->m_pHKeySettings->getChannelSettings();

    // we have 1 hand key input channel
    cHKeyChannel* pChannel;
    pChannel = new cHKeyChannel(m_pSCPIInterface, "Hand key input", 0, channelSettings.at(0) );
    m_ChannelList.append(pChannel);

    m_sVersion = HKeySystem::Version;
}

cHKeyInterface::~cHKeyInterface()
{
    for(auto channel : m_ChannelList) {
        delete channel;
    }
}

void cHKeyInterface::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1HKEY").arg(leadingNodes),"VERSION",SCPI::isQuery, m_pSCPIInterface, HKeySystem::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1HKEY:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, HKeySystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    for(auto channel : m_ChannelList) {
        connect(channel, &cSCPIConnection::strNotifier, this, &cSCPIConnection::strNotifier);
        connect(channel, SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SIGNAL(cmdExecutionDone(cProtonetCommand*)));
        channel->initSCPIConnection(QString("%1HKEY").arg(leadingNodes));
    }
}

void cHKeyInterface::registerResource(cRMConnection *rmConnection, quint16 port)
{
    for(auto channel : m_ChannelList) {
        register1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("HKEY;%1;1;%2;%3;").arg(channel->getName()).arg(channel->getDescription()).arg(port));
    }
}

void cHKeyInterface::unregisterResource(cRMConnection *rmConnection)
{
    for(auto channel : m_ChannelList) {
        unregister1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("HKEY;%1;").arg(channel->getName()));
    }
}

void cHKeyInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case HKeySystem::cmdVersion:
        protoCmd->m_sOutput = m_ReadVersion(protoCmd->m_sInput);
        break;
    case HKeySystem::cmdChannelCat:
        protoCmd->m_sOutput = m_ReadChannelCatalog(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput) {
        emit cmdExecutionDone(protoCmd);
    }
}

QString cHKeyInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        return m_sVersion;
    }
    return SCPI::scpiAnswer[SCPI::nak];
}

QString cHKeyInterface::m_ReadChannelCatalog(QString &sInput)
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
    return SCPI::scpiAnswer[SCPI::nak];
}
