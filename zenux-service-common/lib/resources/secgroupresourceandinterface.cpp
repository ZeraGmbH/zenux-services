#include "secgroupresourceandinterface.h"
#include "scpiconnection.h"
#include "scpisingletonfactory.h"
#include "notzeronumgen.h"
#include <scpi.h>

SecGroupResourceAndInterface::SecGroupResourceAndInterface(int devFileDescriptor,
                                                           SecCalculatorSettings* ecalcSettings,
                                                           SecInputSettings *inputsettings,
                                                           std::function<void (int)> funcSigHandler) :
    cResource(ScpiSingletonFactory::getScpiObj()),
    m_pecalcsettings(ecalcSettings),
    m_pInputSettings(inputsettings)
{
    m_sVersion = ECalcSystem::Version;

    // first we create the configured number of error calculators and attach them into a hash table for better access
    int n = m_pecalcsettings->getNumber();
    for (int i = 0; i < n; i++ ) {
        SecChannel* eChan = new SecChannel(devFileDescriptor, m_pecalcsettings, m_pInputSettings, i, funcSigHandler);
        m_ECalculatorChannelList.append(eChan); // we have a list for seq. access
        m_ECalculatorChannelHash[eChan->getName()] = eChan; // and a hash for access by channel name
        m_ECalculatorChannelList.at(i)->m_StopErrorCalculator(); // initially we stop all ec's
        m_ECalculatorChannelList.at(i)->resetInterrupt(0xF); // and reset all interrupts
    }
}


SecGroupResourceAndInterface::~SecGroupResourceAndInterface()
{
    SecChannel* cptr;

    int n = m_ECalculatorChannelList.count();
    for (int i = 0; i < n; i++)
    {
        cptr = m_ECalculatorChannelList.at(i);
        delete cptr;
    }
}


void SecGroupResourceAndInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1ECALCULATOR").arg(leadingNodes),"VERSION",SCPI::isQuery,m_pSCPIInterface, ECalcSystem::cmdVersion);
    addDelegate(QString("%1ECALCULATOR:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, ECalcSystem::cmdChannelCat);
    addDelegate(QString("%1ECALCULATOR").arg(leadingNodes),"SET",SCPI::CmdwP,m_pSCPIInterface, ECalcSystem::cmdSetChannels);
    addDelegate(QString("%1ECALCULATOR").arg(leadingNodes),"FREE",SCPI::CmdwP,m_pSCPIInterface, ECalcSystem::cmdFreeChannels);
    for (int i = 0; i < m_ECalculatorChannelList.count(); i++) {
        // we also must connect the signals for notification and for output
        connect(m_ECalculatorChannelList.at(i), &ScpiConnection::valNotifier, this, &ScpiConnection::valNotifier);
        connect(m_ECalculatorChannelList.at(i), &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        m_ECalculatorChannelList.at(i)->initSCPIConnection(QString("%1ECALCULATOR").arg(leadingNodes));
    }
}


void SecGroupResourceAndInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
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


void SecGroupResourceAndInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    // we register all our error calculator units as resources
    register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SEC1;ECALCULATOR;%1;%2;%3;")
                      .arg(m_pecalcsettings->getNumber())
                      .arg(ECalcSystem::sECalculatorDescription)
                      .arg(port));

}


void SecGroupResourceAndInterface::unregisterResource(RMConnection *rmConnection)
{
    unregister1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SEC1;ECALCULATOR;"));
}


QList<SecChannel *> SecGroupResourceAndInterface::getECalcChannelList()
{
    return m_ECalculatorChannelList;
}


QString SecGroupResourceAndInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sVersion;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString SecGroupResourceAndInterface::m_ReadECalculatorChannelCatalog(QString &sInput)
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


void SecGroupResourceAndInterface::m_SetChannels(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;

    if (cmd.isCommand(1))
    {
       bool ok;
       int n = cmd.getParam(0).toInt(&ok);
       if (ok && (n > 0) && (n < 5)) // we accept 1 .. 4 ecalc requests
       {
           QString s;
           QList<SecChannel*> selEChannels;

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


void SecGroupResourceAndInterface::m_FreeChannels(cProtonetCommand *protoCmd)
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






