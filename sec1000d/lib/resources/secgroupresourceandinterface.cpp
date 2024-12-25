#include "secgroupresourceandinterface.h"
#include "scpiconnection.h"
#include "zscpi_response_definitions.h"
#include "notzeronumgen.h"
#include <scpi.h>

SecGroupResourceAndInterface::SecGroupResourceAndInterface(std::shared_ptr<cSCPI> scpiInterface,
                                                           SecCalculatorSettings* ecalcSettings,
                                                           SecInputSettings *inputsettings,
                                                           std::function<void (int)> funcSigHandler,
                                                           AbstractFactoryDeviceNodeSecPtr deviceNodeFactory) :
    cResource(scpiInterface),
    m_pecalcsettings(ecalcSettings),
    m_pInputSettings(inputsettings)
{
    m_sVersion = ECalcSystem::Version;

    // first we create the configured number of error calculators and attach them into a hash table for better access
    int n = m_pecalcsettings->getNumber();
    for (int i = 0; i < n; i++ ) {
        SecChannel* eChan = new SecChannel(m_scpiInterface,
                                           m_pecalcsettings,
                                           m_pInputSettings,
                                           i,
                                           funcSigHandler,
                                           deviceNodeFactory);
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
    addDelegate(QString("%1ECALCULATOR").arg(leadingNodes),"VERSION",SCPI::isQuery,m_scpiInterface, ECalcSystem::cmdVersion);
    addDelegate(QString("%1ECALCULATOR:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_scpiInterface, ECalcSystem::cmdChannelCat);
    addDelegate(QString("%1ECALCULATOR").arg(leadingNodes),"SET",SCPI::CmdwP,m_scpiInterface, ECalcSystem::cmdSetChannels);
    addDelegate(QString("%1ECALCULATOR").arg(leadingNodes),"FREE",SCPI::CmdwP,m_scpiInterface, ECalcSystem::cmdFreeChannels);
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


QList<SecChannel *> SecGroupResourceAndInterface::getECalcChannelList()
{
    return m_ECalculatorChannelList;
}

bool SecGroupResourceAndInterface::freeChannelsForThisPeer(VeinTcp::TcpPeer *peer)
{
    bool result = true;
    for(const QByteArray &client : qAsConst(m_peerClientsHash[peer]))
        if(!freeChannelsFromAClient(client))
            result = false;
    return result;
}

QString SecGroupResourceAndInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sVersion;
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
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
        return ZSCPI::scpiAnswer[ZSCPI::nak];
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
               if(!m_peerClientsHash[protoCmd->m_pPeer].contains(id))
                   m_peerClientsHash[protoCmd->m_pPeer].append(id);
               for (int i = 0; i < selEChannels.count(); i++)
                   selEChannels.at(i)->set(id);
           }
           else
               protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::busy];
       }
       else
           protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];

}


void SecGroupResourceAndInterface::m_FreeChannels(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak]; // preset
    if (cmd.isCommand(0))
    {
       if (freeChannelsFromAClient(protoCmd->m_clientId))
           protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
    }
}

bool SecGroupResourceAndInterface::freeChannelsFromAClient(QByteArray clientID)
{
    bool result = false;
    if (m_ClientECalcHash.contains(clientID))
    {
       QStringList sl = m_ClientECalcHash[clientID].split(";");
       for (int i = 0; i < sl.count(); i++)
       {
           QString key;
           key = sl.at(i);
           if (m_ECalculatorChannelHash.contains(key))
               m_ECalculatorChannelHash[key]->free();
       }
       result = true;
    }
    return result;
}
