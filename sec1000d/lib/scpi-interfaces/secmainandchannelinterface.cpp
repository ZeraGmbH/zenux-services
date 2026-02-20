#include "secmainandchannelinterface.h"
#include "scpiconnection.h"
#include "zscpi_response_definitions.h"
#include <scpi.h>

enum Commands
{
    cmdVersion,
    cmdChannelCat,
    cmdSetChannels,
    cmdFreeChannels
};

SecMainAndChannelInterface::SecMainAndChannelInterface(std::shared_ptr<cSCPI> scpiInterface,
                                                           SecCalculatorSettings* ecalcSettings,
                                                           SecInputSettings *inputsettings,
                                                           std::function<void (int)> funcSigHandler,
                                                           AbstractFactoryDeviceNodeSecPtr zdspSupportFactory) :
    ScpiServerConnection(scpiInterface),
    m_pecalcsettings(ecalcSettings),
    m_pInputSettings(inputsettings)
{
    for (int channelNo = 0; channelNo < m_pecalcsettings->getNumber(); channelNo++) {
        SecChannel* eChan = new SecChannel(m_scpiInterface,
                                           m_pecalcsettings,
                                           m_pInputSettings,
                                           channelNo,
                                           funcSigHandler,
                                           zdspSupportFactory);
        m_ECalculatorChannelList.append(eChan);
        eChan->m_StopErrorCalculator();
        eChan->resetInterrupt(0xF);
    }
}

SecMainAndChannelInterface::~SecMainAndChannelInterface()
{
    for (SecChannel* channel : m_ECalculatorChannelList)
        delete channel;
}

void SecMainAndChannelInterface::connectChannelSignalsAndInitScpi()
{
    for (int i = 0; i < m_ECalculatorChannelList.count(); i++) {
        connect(m_ECalculatorChannelList.at(i), &ScpiConnection::valNotifier, this, &ScpiConnection::valNotifier);
        connect(m_ECalculatorChannelList.at(i), &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        m_ECalculatorChannelList.at(i)->initSCPIConnection("ECALCULATOR");
    }
}

void SecMainAndChannelInterface::initSCPIConnection()
{
    addDelegate("ECALCULATOR", "VERSION",SCPI::isQuery, m_scpiInterface, cmdVersion);
    addDelegate("ECALCULATOR:CHANNEL", "CATALOG", SCPI::isQuery, m_scpiInterface, cmdChannelCat);
    addDelegate("ECALCULATOR", "SET",SCPI::CmdwP, m_scpiInterface, cmdSetChannels);
    addDelegate("ECALCULATOR", "FREE",SCPI::CmdwP, m_scpiInterface, cmdFreeChannels);

    connectChannelSignalsAndInitScpi();
}

void SecMainAndChannelInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
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

QList<SecChannel *> SecMainAndChannelInterface::getECalcChannelList()
{
    return m_ECalculatorChannelList;
}

bool SecMainAndChannelInterface::freeChannelsForThisPeer(VeinTcp::TcpPeer *peer)
{
    for(const QByteArray &client : qAsConst(m_peerClientsHash[peer]))
        if(!freeChannelsFromAClient(client))
            return false;
    return true;
}

QString SecMainAndChannelInterface::scpiReadVersion(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return ECalcSystem::Version;
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SecMainAndChannelInterface::scpiReadECalculatorChannelCatalog(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        QString ret;
        for (int i = 0; i < m_ECalculatorChannelList.count(); i++)
            ret += QString("%1;").arg(m_ECalculatorChannelList.at(i)->getName());
        return ret;
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

void SecMainAndChannelInterface::scpiSetChannels(ProtonetCommandPtr protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;

    if (cmd.isCommand(1))
    {
       bool ok;
       int n = cmd.getParam(0).toInt(&ok);
       if (ok && (n > 0) && (n <= m_ECalculatorChannelList.count()))
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


void SecMainAndChannelInterface::scpiFreeChannels(ProtonetCommandPtr protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak]; // preset
    if (cmd.isCommand(0)) {
       if (freeChannelsFromAClient(protoCmd->m_clientId))
           protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
    }
}

bool SecMainAndChannelInterface::freeChannelsFromAClient(QByteArray clientID)
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
