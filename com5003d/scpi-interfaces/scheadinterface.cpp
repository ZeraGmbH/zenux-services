#include "scheadinterface.h"
#include <scpiconnection.h>
#include "resource.h"
#include "com5003d.h"
#include "scheadchannel.h"
#include <protonetcommand.h>
#include "settings/scheadsettings.h"
#include <xmlsettings.h>
#include <scpi.h>


cSCHeadInterface::cSCHeadInterface(cCOM5003dServer *server) :
    cResource(server->getSCPIInterface())
{
    m_pMyServer = server;
    QList<SCHeadSystem::cChannelSettings*> channelSettings;
    channelSettings = m_pMyServer->m_pSCHeadSettings->getChannelSettings();

    // we have 1 scanning head input channel
    cSCHeadChannel* pChannel;
    pChannel = new cSCHeadChannel(m_pSCPIInterface, "Scanning head input", 0, channelSettings.at(0) );
    m_ChannelList.append(pChannel);

    m_sVersion = SCHeadSystem::Version;
}


cSCHeadInterface::~cSCHeadInterface()
{
    cSCHeadChannel* cptr;
    for ( int i = 0; i < m_ChannelList.count(); i++)
    {
        cptr = m_ChannelList.at(i);
        delete cptr;
    }
}


void cSCHeadInterface::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1SCHEAD").arg(leadingNodes),"VERSION",SCPI::isQuery,m_pSCPIInterface, SCHeadSystem::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SCHEAD:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SCHeadSystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));

    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        connect(m_ChannelList.at(i), &cSCPIConnection::strNotifier, this, &cSCPIConnection::strNotifier);
        connect(m_ChannelList.at(i), SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SIGNAL(cmdExecutionDone(cProtonetCommand*)));
        m_ChannelList.at(i)->initSCPIConnection(QString("%1SCHEAD").arg(leadingNodes));
    }
}


void cSCHeadInterface::registerResource(cRMConnection *rmConnection, quint16 port)
{
    cSCHeadChannel* pChannel;
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        pChannel = m_ChannelList.at(i);
        register1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SCHEAD;%1;1;%2;%3;").arg(pChannel->getName()).arg(pChannel->getDescription()).arg(port));
    }
}


void cSCHeadInterface::unregisterResource(cRMConnection *rmConnection)
{
    cSCHeadChannel* pChannel;
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        pChannel = m_ChannelList.at(i);
        unregister1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SCHEAD;%1;").arg(pChannel->getName()));
    }
}


void cSCHeadInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SCHeadSystem::cmdVersion:
        protoCmd->m_sOutput = m_ReadVersion(protoCmd->m_sInput);
        break;
    case SCHeadSystem::cmdChannelCat:
        protoCmd->m_sOutput = m_ReadChannelCatalog(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString cSCHeadInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sVersion;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSCHeadInterface::m_ReadChannelCatalog(QString &sInput)
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






