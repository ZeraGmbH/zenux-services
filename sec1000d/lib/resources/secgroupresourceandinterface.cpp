#include "secgroupresourceandinterface.h"
#include "scpiconnection.h"
#include "zscpi_response_definitions.h"
#include "notzeronumgen.h"
#include <scpi.h>

enum Commands
{
    cmdVersion,
    cmdChannelCat,
    cmdSetChannels,
    cmdFreeChannels
};

SecGroupResourceAndInterface::SecGroupResourceAndInterface(std::shared_ptr<cSCPI> scpiInterface,
                                                           SecCalculatorSettings* ecalcSettings,
                                                           SecInputSettings *inputsettings,
                                                           std::function<void (int)> funcSigHandler,
                                                           AbstractFactoryDeviceNodeSecPtr deviceNodeFactory) :
    cResource(scpiInterface),
    m_pecalcsettings(ecalcSettings),
    m_pInputSettings(inputsettings)
{
    for (int channelNo = 0; channelNo < m_pecalcsettings->getNumber(); channelNo++) {
        SecChannel* eChan = new SecChannel(m_scpiInterface,
                                           m_pecalcsettings,
                                           m_pInputSettings,
                                           channelNo,
                                           funcSigHandler,
                                           deviceNodeFactory);
        m_ECalculatorChannelList.append(eChan);
        eChan->m_StopErrorCalculator();
        eChan->resetInterrupt(0xF);
    }
}

SecGroupResourceAndInterface::~SecGroupResourceAndInterface()
{
    for (SecChannel* channel : m_ECalculatorChannelList)
        delete channel;
}

void SecGroupResourceAndInterface::connectChannelSignalsAndInitScpi(const QString &leadingNodes)
{
    for (int i = 0; i < m_ECalculatorChannelList.count(); i++) {
        connect(m_ECalculatorChannelList.at(i), &ScpiConnection::valNotifier, this, &ScpiConnection::valNotifier);
        connect(m_ECalculatorChannelList.at(i), &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        m_ECalculatorChannelList.at(i)->initSCPIConnection(QString("%1ECALCULATOR").arg(leadingNodes));
    }
}

void SecGroupResourceAndInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1ECALCULATOR").arg(leadingNodes),"VERSION",SCPI::isQuery,m_scpiInterface, cmdVersion);
    addDelegate(QString("%1ECALCULATOR:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_scpiInterface, cmdChannelCat);
    addDelegate(QString("%1ECALCULATOR").arg(leadingNodes),"SET",SCPI::CmdwP,m_scpiInterface, cmdSetChannels);
    addDelegate(QString("%1ECALCULATOR").arg(leadingNodes),"FREE",SCPI::CmdwP,m_scpiInterface, cmdFreeChannels);

    connectChannelSignalsAndInitScpi(leadingNodes);
}

void SecGroupResourceAndInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case cmdVersion:
        protoCmd->m_sOutput = scpiReadVersion(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case cmdChannelCat:
        protoCmd->m_sOutput = scpiReadECalculatorChannelCatalog(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case cmdSetChannels:
        scpiSetChannels(protoCmd);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case cmdFreeChannels:
        scpiFreeChannels(protoCmd);
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

QString SecGroupResourceAndInterface::scpiReadVersion(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return ECalcSystem::Version;
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString SecGroupResourceAndInterface::scpiReadECalculatorChannelCatalog(const QString &scpi)
{
    cSCPICommand cmd = scpi;

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


void SecGroupResourceAndInterface::scpiSetChannels(cProtonetCommand *protoCmd)
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


void SecGroupResourceAndInterface::scpiFreeChannels(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak]; // preset
    if (cmd.isCommand(0)) {
       if (freeChannelsFromAClient(protoCmd->m_clientId))
           protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
    }
}

bool SecGroupResourceAndInterface::freeChannelsFromAClient(QByteArray clientID)
{
    if (m_ClientECalcHash.contains(clientID)) {
        QStringList clientEcChannels = m_ClientECalcHash[clientID].split(";");
        for (const QString &ecChannel : clientEcChannels) {
            for (SecChannel* channel : m_ECalculatorChannelList) {
                if (channel->getName() == ecChannel)
                    channel->free();
            }
        }
        return true;
    }
    return false;
}
