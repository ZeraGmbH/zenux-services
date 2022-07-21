#include "ecalcinterface.h"
#include <scpiconnection.h>
#include "sec1000d.h"
#include "protonetcommand.h"
#include <scpi.h>
#include <xmlsettings.h>

cECalculatorInterface::cECalculatorInterface(cSEC1000dServer* server, cETHSettings* ethsettings, cECalculatorSettings* ecalcSettings, cFPGASettings* fpgasettings, cInputSettings *inputsettings) :
    cResource(ScpiSingletonFactory::getScpiObj(ServerName)),
    m_pMyServer(server),
    m_pETHsettings(ethsettings),
    m_pecalcsettings(ecalcSettings),
    m_pfpgasettings(fpgasettings),
    m_pInputSettings(inputsettings)
{
    m_sVersion = ECalcSystem::Version;

    // first we create the configured number of error calculators and attach them into a hash table for better access
    int n = m_pecalcsettings->getNumber();
    for (int i = 0; i < n; i++ )
    {
        cECalculatorChannel* eChan = new cECalculatorChannel(m_pMyServer, m_pecalcsettings, m_pfpgasettings, m_pInputSettings, i);
        m_ECalculatorChannelList.append(eChan); // we have a list for seq. access
        m_ECalculatorChannelHash[eChan->getName()] = eChan; // and a hash for access by channel name
        m_ECalculatorChannelList.at(i)->m_StopErrorCalculator(); // initially we stop all ec's
        m_ECalculatorChannelList.at(i)->m_resetInterrupt(0xF); // and reset all interrupts

    }
}


cECalculatorInterface::~cECalculatorInterface()
{
    cECalculatorChannel* cptr;

    int n = m_ECalculatorChannelList.count();
    for (int i = 0; i < n; i++)
    {
        cptr = m_ECalculatorChannelList.at(i);
        delete cptr;
    }
}


void cECalculatorInterface::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1ECALCULATOR").arg(leadingNodes),"VERSION",SCPI::isQuery,m_pSCPIInterface, ECalcSystem::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1ECALCULATOR:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, ECalcSystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1ECALCULATOR").arg(leadingNodes),"SET",SCPI::CmdwP,m_pSCPIInterface, ECalcSystem::cmdSetChannels);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1ECALCULATOR").arg(leadingNodes),"FREE",SCPI::CmdwP,m_pSCPIInterface, ECalcSystem::cmdFreeChannels);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));

    int n = m_ECalculatorChannelList.count();
    for (int i = 0; i < n; i++)
    {
        // we also must connect the signals for notification and for output
        connect(m_ECalculatorChannelList.at(i), &cSCPIConnection::valNotifier, this, &cSCPIConnection::valNotifier);
        connect(m_ECalculatorChannelList.at(i), SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SIGNAL(cmdExecutionDone(cProtonetCommand*)));

        m_ECalculatorChannelList.at(i)->initSCPIConnection(QString("%1ECALCULATOR").arg(leadingNodes));
    }
}


void cECalculatorInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case ECalcSystem::cmdVersion:
        protoCmd->m_sOutput = m_ReadVersion(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case ECalcSystem::cmdChannelCat:
        protoCmd->m_sOutput = m_ReadECalculatorChannelCatalog(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            if (protoCmd->m_bwithOutput)
                emit cmdExecutionDone(protoCmd);
        break;
    case ECalcSystem::cmdSetChannels:
        m_SetChannels(protoCmd);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case ECalcSystem::cmdFreeChannels:
        m_FreeChannels(protoCmd);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    }
}


void cECalculatorInterface::registerResource(cRMConnection *rmConnection, quint16 port)
{
    // we register all our error calculator units as resources
    register1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SEC1;ECALCULATOR;%1;%2;%3;")
                      .arg(m_pecalcsettings->getNumber())
                      .arg(ECalcSystem::sECalculatorDescription)
                      .arg(port));

}


void cECalculatorInterface::unregisterResource(cRMConnection *rmConnection)
{
    unregister1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SEC1;ECALCULATOR;"));
}


QList<cECalculatorChannel *> cECalculatorInterface::getECalcChannelList()
{
    return m_ECalculatorChannelList;
}


QString cECalculatorInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sVersion;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cECalculatorInterface::m_ReadECalculatorChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        QString s;
        int n = m_ECalculatorChannelList.count();
        for (int i = 0; i < n; i++)
            s += QString("%1;").arg(m_ECalculatorChannelList.at(i)->getName());

        return s;
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


void cECalculatorInterface::m_SetChannels(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;

    if (cmd.isCommand(1))
    {
       bool ok;
       int n = cmd.getParam(0).toInt(&ok);
       if (ok && (n > 0) && (n < 5)) // we accept 1 .. 4 ecalc requests
       {
           QString s;
           QList<cECalculatorChannel*> selEChannels;

           int m = m_ECalculatorChannelList.count();
           for (int i = 0; i < m; i++)
           {
               if (m_ECalculatorChannelList.at(i)->isfree())
               {
                   selEChannels.append(m_ECalculatorChannelList.at(i));
                   s += QString("%1;").arg(m_ECalculatorChannelList.at(i)->getName());
                   n--;
               }
               if (n == 0)
                   break;
           }
           if (n == 0)
           {
               protoCmd->m_sOutput = s;
               QByteArray id = protoCmd->m_clientId;
               m_ClientECalcHash[id] = s;
               for (int i = 0; i < selEChannels.count(); i++)
                   selEChannels.at(i)->set(id);
           }
           else
               protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::busy];
       }
       else
           protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
    }
    else
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];

}


void cECalculatorInterface::m_FreeChannels(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;

    protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak]; // preset

    if (cmd.isCommand(0))
    {
       if (m_ClientECalcHash.contains(protoCmd->m_clientId))
       {
           QStringList sl = m_ClientECalcHash[protoCmd->m_clientId].split(";");
           for (int i = 0; i < sl.count(); i++)
           {
               QString key;
               key = sl.at(i);
               if (m_ECalculatorChannelHash.contains(key))
                   m_ECalculatorChannelHash[key]->free();
           }
           protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack];
       }
    }
}






