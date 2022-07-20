#include "frqinputinterface.h"
#include <scpiconnection.h>
#include "resource.h"
#include "com5003d.h"
#include "fpzinchannel.h"
#include <protonetcommand.h>
#include "settings/frqinputsettings.h"
#include <xmlsettings.h>
#include <scpi.h>

cFRQInputInterface::cFRQInputInterface(cCOM5003dServer *server) :
    cResource(server->getSCPIInterface()),
    m_pMyServer(server)
{
    QList<FRQInputSystem::cChannelSettings*> channelSettings;
    channelSettings = m_pMyServer->m_pFRQInputSettings->getChannelSettings();

    // we have 4 frequency input channels
    cFPZInChannel* pChannel;
    pChannel = new cFPZInChannel(m_pMyServer, "Frequency input 0..1MHz", 0, channelSettings.at(0) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZInChannel(m_pMyServer, "Frequency output 0..1MHz", 1, channelSettings.at(1) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZInChannel(m_pMyServer, "Frequency output 0..1MHz", 2,  channelSettings.at(2) );
    m_ChannelList.append(pChannel);
    pChannel = new cFPZInChannel(m_pMyServer, "Frequency output 0..1MHz", 3, channelSettings.at(3) );
    m_ChannelList.append(pChannel);

    m_sVersion = FRQInputSystem::Version;
}


cFRQInputInterface::~cFRQInputInterface()
{
    cFPZInChannel* cptr;
    for ( int i = 0; i < m_ChannelList.count(); i++)
    {
        cptr = m_ChannelList.at(i);
        delete cptr;
    }
}


void cFRQInputInterface::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1FRQINPUT").arg(leadingNodes),"VERSION",SCPI::isQuery,m_pSCPIInterface, FRQInputSystem::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1FRQINPUT:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, FRQInputSystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));

    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        connect(m_ChannelList.at(i), &cSCPIConnection::strNotifier, this, &cSCPIConnection::strNotifier);
        connect(m_ChannelList.at(i), SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SIGNAL(cmdExecutionDone(cProtonetCommand*)));
        m_ChannelList.at(i)->initSCPIConnection(QString("%1FRQINPUT").arg(leadingNodes));
    }
}


void cFRQInputInterface::registerResource(cRMConnection *rmConnection, quint16 port)
{
    cFPZInChannel* pChannel;
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        pChannel = m_ChannelList.at(i);
        register1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("FRQINPUT;%1;1;%2;%3;").arg(pChannel->getName()).arg(pChannel->getDescription()).arg(port));
    }
}


void cFRQInputInterface::unregisterResource(cRMConnection *rmConnection)
{
    cFPZInChannel* pChannel;
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        pChannel = m_ChannelList.at(i);
        unregister1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("FRQINPUT;%1;").arg(pChannel->getName()));
    }
}


void cFRQInputInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case FRQInputSystem::cmdVersion:
        protoCmd->m_sOutput = m_ReadVersion(protoCmd->m_sInput);
        break;
    case FRQInputSystem::cmdChannelCat:
        protoCmd->m_sOutput = m_ReadChannelCatalog(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString cFRQInputInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sVersion;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cFRQInputInterface::m_ReadChannelCatalog(QString &sInput)
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





