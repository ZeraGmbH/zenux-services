#include "sourceinterface.h"
#include <scpiconnection.h>
#include "resource.h"
#include "com5003d.h"
#include "fpzchannel.h"
#include <protonetcommand.h>
#include "settings/sourcesettings.h"
#include <xmlsettings.h>
#include <scpi.h>


cSourceInterface::cSourceInterface(cCOM5003dServer *server) :
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
    pChannel = new cFPZChannel(m_pMyServer->getSCPIInterface(), "Reference frequency output 0..1MHz", 2,  channelSettings.at(2) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZChannel(m_pMyServer->getSCPIInterface(), "Reference frequency output 0..1MHz", 3, channelSettings.at(3) );
    m_ChannelList.append(pChannel);

    m_sVersion = SourceSystem::Version;
}


cSourceInterface::~cSourceInterface()
{
    cFPZChannel* cptr;
    for ( int i = 0; i < m_ChannelList.count(); i++)
    {
        cptr = m_ChannelList.at(i);
        delete cptr;
    }
}


void cSourceInterface::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1SOURCE").arg(leadingNodes),"VERSION",SCPI::isQuery,m_pSCPIInterface, SourceSystem::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SOURCE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SourceSystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));

    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        connect(m_ChannelList.at(i), &cSCPIConnection::strNotifier, this, &cSCPIConnection::strNotifier);
        connect(m_ChannelList.at(i), SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SIGNAL(cmdExecutionDone(cProtonetCommand*)));
        m_ChannelList.at(i)->initSCPIConnection(QString("%1SOURCE").arg(leadingNodes));
    }
}


void cSourceInterface::registerResource(cRMConnection *rmConnection, quint16 port)
{
    cFPZChannel* pChannel;
    for (int i = 0; i < 4; i++)
    {
        pChannel = m_ChannelList.at(i);
        register1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SOURCE;%1;1;%2;%3;").arg(pChannel->getName()).arg(pChannel->getDescription()).arg(port));
    }
}


void cSourceInterface::unregisterResource(cRMConnection *rmConnection)
{
    cFPZChannel* pChannel;
    for (int i = 0; i < 4; i++)
    {
        pChannel = m_ChannelList.at(i);
        unregister1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SOURCE;%1;").arg(pChannel->getName()));
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




