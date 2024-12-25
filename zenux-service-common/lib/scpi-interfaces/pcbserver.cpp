#include "pcbserver.h"
#include "zscpi_response_definitions.h"
#include <scpi.h>
#include <fcntl.h>
#include <unistd.h>
#include <QtDebug>
#include <QFile>
#include <QByteArray>
#include <QList>
#include <QString>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDataStream>

enum commands
{
    cmdRegister,
    cmdUnregister
};

PCBServer::PCBServer(SettingsContainerPtr settings,
                     VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory) :
    ScpiConnection(std::make_shared<cSCPI>()),
    m_tcpNetworkFactory(tcpNetworkFactory),
    m_settings(std::move(settings))
{
}

void PCBServer::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SERVER").arg(leadingNodes), "REGISTER", SCPI::isCmdwP, m_scpiInterface, cmdRegister);
    addDelegate(QString("%1SERVER").arg(leadingNodes), "UNREGISTER", SCPI::isQuery | SCPI::isCmd, m_scpiInterface, cmdUnregister);
}

std::shared_ptr<cSCPI> PCBServer::getSCPIInterface()
{
    return m_scpiInterface;
}

QString PCBServer::getName()
{
    return m_settings->getServerParams().name;
}

QString PCBServer::getVersion()
{
    return m_settings->getServerParams().version;
}

void PCBServer::setupServer()
{
    m_protoBufServer = new VeinTcp::TcpServer(m_tcpNetworkFactory, this);
    connect(m_protoBufServer,&VeinTcp::TcpServer::sigClientConnected,this,&PCBServer::onProtobufClientConnected);
}

void PCBServer::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case cmdRegister:
        registerNotifier(protoCmd);
        break;
    case cmdUnregister:
        unregisterNotifier(protoCmd);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

void PCBServer::openTelnetScpi()
{
    EthSettings *ethSettings = m_settings->getEthSettings();
    if(ethSettings->isSCPIactive()) {
        m_telnetServer = new QTcpServer();
        m_telnetServer->setMaxPendingConnections(1); // we only accept 1 client to connect
        connect(m_telnetServer, &QTcpServer::newConnection, this, &PCBServer::onTelnetClientConnected);
        m_telnetServer->listen(QHostAddress::AnyIPv4, ethSettings->getPort(EthSettings::scpiserver));
    }
}

void PCBServer::sendAnswerProto(cProtonetCommand *protoCmd)
{
    if(protoCmd->m_pPeer == 0) {
        // we worked on a command comming from scpi socket connection
        QString answer = protoCmd->m_sOutput+"\n";
        QByteArray ba = answer.toLatin1();
        m_telnetSocket->write(ba);
        qInfo("External SCPI response: %s", qPrintable(answer));
    }
    else {
        if(protoCmd->m_bhasClientId) {
            ProtobufMessage::NetMessage protobufAnswer;
            ProtobufMessage::NetMessage::NetReply *Answer = protobufAnswer.mutable_reply();
            // dependent on rtype caller can see ack, nak, error
            // in case of error the body has to be analyzed for details
            QString output = protoCmd->m_sOutput;
            if (output.contains(ZSCPI::scpiAnswer[ZSCPI::ack]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);
            else if (output.contains(ZSCPI::scpiAnswer[ZSCPI::nak]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_NACK);
            else if (output.contains(ZSCPI::scpiAnswer[ZSCPI::busy]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(ZSCPI::scpiAnswer[ZSCPI::erraut]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(ZSCPI::scpiAnswer[ZSCPI::errval]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(ZSCPI::scpiAnswer[ZSCPI::errxml]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(ZSCPI::scpiAnswer[ZSCPI::errpath])) // for zdspd only -> remove
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(ZSCPI::scpiAnswer[ZSCPI::errexec]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);

            Answer->set_body(output.toStdString()); // in any case we set the body
            protobufAnswer.set_clientid(protoCmd->m_clientId, protoCmd->m_clientId.count());
            protobufAnswer.set_messagenr(protoCmd->m_nmessageNr);
            protoCmd->m_pPeer->sendMessage(m_protobufWrapper.protobufToByteArray(protobufAnswer));
        }
        else {
            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_0);
            out << (qint32)0;

            out << protoCmd->m_sOutput.toUtf8();
            out.device()->seek(0);
            out << (qint32)(block.size() - sizeof(qint32));

            protoCmd->m_pPeer->writeRaw(block);
        }
    }
    delete protoCmd;
}

void PCBServer::onTelnetClientConnected()
{
    qInfo("External SCPI Client connected");
    m_telnetSocket = m_telnetServer->nextPendingConnection();
    connect(m_telnetSocket, &QIODevice::readyRead, this, &PCBServer::onTelnetDataReceived);
    connect(m_telnetSocket, &QAbstractSocket::disconnected, this, &PCBServer::onTelnetDisconnect);
}

void PCBServer::onTelnetDataReceived()
{
    QString input;
    while(m_telnetSocket->canReadLine())
        input += m_telnetSocket->readLine();
    input.remove('\r'); // we remove cr lf
    input.remove('\n');
    qInfo("External SCPI command: %s", qPrintable(input));

    QByteArray clientId = QByteArray(); // we set an empty byte array
    cProtonetCommand* protoCmd = new cProtonetCommand(0, false, true, clientId, 0, input);
    // peer = 0 means we are working on the scpi socket ....
    cSCPIObject* scpiObject = m_scpiInterface->getSCPIObject(input);
    if(scpiObject) {
        cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
        if (!scpiDelegate->executeSCPI(protoCmd)) {
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
            emit cmdExecutionDone(protoCmd);
        }
    }
    else {
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
        emit cmdExecutionDone(protoCmd);
    }
}

void PCBServer::onTelnetDisconnect()
{
    qInfo("External SCPI Client disconnected");
    disconnect(m_telnetSocket, 0, 0, 0); // we disconnect everything
}

void PCBServer::onNotifySubscriber(ScpiNotificationSubscriber subscriber, QString newValue)
{
    QString notificationMsg = QString("Notify:%1").arg(subscriber.m_notifierId);
    if(subscriber.m_notifyWithValue)
        notificationMsg.append(QString(":%1").arg(newValue));
    sendNotificationToClient(notificationMsg, subscriber.m_clientId, subscriber.m_netPeer);
}

void PCBServer::onProtobufDisconnect(VeinTcp::TcpPeer *peer)
{
    Q_UNUSED(peer)
}

void PCBServer::registerNotifier(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if(cmd.isCommand(1)) { // SEC service receives registerNotifier with 1 param
        QString query = cmd.getParam(0);
        cSCPIObject* scpiObject = m_scpiInterface->getSCPIObject(query);
        if(scpiObject) {
            NotificationStructWithValue notData;
            notData.netPeer = protoCmd->m_pPeer;
            notData.clientID = protoCmd->m_clientId;
            connect(notData.netPeer, &VeinTcp::TcpPeer::sigConnectionClosed, this, &PCBServer::onNotifyPeerConnectionClosed);
            m_notifierRegisterNext.append(notData); // we wait for a notifier signal

            cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
            cProtonetCommand* procmd = new cProtonetCommand(protoCmd);
            procmd->m_nSCPIType = SCPI::isQuery; // we need to set query type for proper execution
            procmd->m_bwithOutput = false;
            procmd->m_sInput = query;
            if (!scpiDelegate->executeSCPI(procmd)) {
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
                m_notifierRegisterNext.pop_back();
            }
            else
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack]; // we overwrite the query's output here
        }
        else
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
    }
    else if(cmd.isCommand(2) || cmd.isCommand(3)) {// mt310s2/com5003 service receives registerNotifier with 2/3 params
        QString query = cmd.getParam(0);
        cSCPIObject* scpiObject = m_scpiInterface->getSCPIObject(query);
        if(scpiObject) {
            connect( protoCmd->m_pPeer, &VeinTcp::TcpPeer::sigConnectionClosed, this, &PCBServer::onNotifyPeerConnectionClosed);
            cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
            cProtonetCommand* procmd = new cProtonetCommand(protoCmd);
            procmd->m_bwithOutput = false;
            procmd->m_sInput = query;
            int notifyId = cmd.getParam(1).toInt();
            bool notifyWithValue = false;
            if(cmd.isCommand(3))
                notifyWithValue = cmd.getParam(2).toInt();
            ScpiNotificationSubscriber subscriber(protoCmd->m_pPeer, protoCmd->m_clientId, notifyId, notifyWithValue);
            scpiDelegate->getScpiNotificationSubscriberHandler().addSubscriber(subscriber);
            scpiDelegate->executeSCPI(procmd);
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack]; // we overwrite the query's output here
            emit notifierRegistred(scpiDelegate->getNotificationString());
        }
        else
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
    }
}

void PCBServer::unregisterNotifier(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if(cmd.isCommand(1) && (cmd.getParam(0) == "") ) {
        doUnregisterNotifier(protoCmd->m_pPeer, protoCmd->m_clientId);
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
}

void PCBServer::doUnregisterNotifier(VeinTcp::TcpPeer* peer, const QByteArray &clientID)
{
    emit removeSubscribers(peer, clientID);
    if(m_notifierRegisterList.count() > 0) {
        // we have to remove all notifiers for this client and or clientId
        // iterate backwards so removals do not confuse our loop
        for(int i = m_notifierRegisterList.count()-1; i >= 0; i--) {
            NotificationStructWithValue notData = m_notifierRegisterList.at(i);
            if(peer == notData.netPeer) {
                // we found the client
                if(clientID.isEmpty() || notData.clientID.isEmpty() || (notData.clientID == clientID)) {
                    m_notifierRegisterList.removeAt(i);
                }
            }
        }
    }
}

void PCBServer::onProtobufClientConnected(VeinTcp::TcpPeer *newClient)
{
    connect(newClient, &VeinTcp::TcpPeer::sigMessageReceived, this, &PCBServer::onProtobufDataReceived);
    connect(newClient, &VeinTcp::TcpPeer::sigConnectionClosed, this, &PCBServer::onProtobufDisconnect);
}

void PCBServer::onProtobufDataReceived(VeinTcp::TcpPeer *peer, QByteArray message)
{
    executeCommandProto(peer, m_protobufWrapper.byteArrayToProtobuf(message));
}

void PCBServer::executeCommandProto(VeinTcp::TcpPeer* peer, std::shared_ptr<google::protobuf::Message> cmd)
{
    cSCPIObject* scpiObject;
    std::shared_ptr<ProtobufMessage::NetMessage> protobufCommand = std::static_pointer_cast<ProtobufMessage::NetMessage>(cmd);
    if ( (protobufCommand != nullptr) && (peer != nullptr)) {
        if (protobufCommand->has_clientid()) {
            QByteArray clientId = QByteArray(protobufCommand->clientid().data(), protobufCommand->clientid().size());
            if (protobufCommand->has_netcommand()) {
                // in case of "lost" clients we delete registration for notification
                doUnregisterNotifier(peer, clientId);
            }
            else if (protobufCommand->has_messagenr()) {
                quint32 messageNr = protobufCommand->messagenr();
                ProtobufMessage::NetMessage::ScpiCommand scpiCmd = protobufCommand->scpi();
                QString input = QString::fromStdString(scpiCmd.command()) +  " " + QString::fromStdString(scpiCmd.parameter());
                cProtonetCommand* protoCmd;
                scpiObject =  m_scpiInterface->getSCPIObject(input);
                if (scpiObject) {
                    protoCmd = new cProtonetCommand(peer, true, true, clientId, messageNr, input, scpiObject->getType());
                    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
                    if (!scpiDelegate->executeSCPI(protoCmd)) {
                        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
                        emit cmdExecutionDone(protoCmd);
                    }
                }
                else {
                    QString cmd =  QString::fromStdString(protobufCommand->scpi().command());
                    protoCmd = new cProtonetCommand(peer, true, true, clientId, messageNr, cmd);
                    protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
                    emit cmdExecutionDone(protoCmd);
                }
                // we get a signal when a command is finished and send answer then
            }
        }
        else {
            QString input =  QString::fromStdString(protobufCommand->scpi().command());
            QByteArray clientId = QByteArray(); // we set an empty byte array
            cProtonetCommand* protoCmd;
            scpiObject =  m_scpiInterface->getSCPIObject(input);
            if (scpiObject) {
                protoCmd = new cProtonetCommand(peer, false, true, clientId, 0, input, scpiObject->getType());
                cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
                if (!scpiDelegate->executeSCPI(protoCmd)) {
                    protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
                    emit cmdExecutionDone(protoCmd);
                }
            }
            else {
                protoCmd = new cProtonetCommand(peer, false, true, clientId, 0, input, 0);
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
                emit cmdExecutionDone(protoCmd);
            }
        }
    }
}

void PCBServer::onEstablishNewNotifier(NotificationValue *notifier)
{
    if (m_notifierRegisterNext.count() > 0) // if we're waiting for notifier
    {
        disconnect(notifier, 0, 0, 0); // we disconnect first because we only want 1 signal
        NotificationStructWithValue notData = m_notifierRegisterNext.takeFirst(); // we pick the notification data
        notData.notValue = notifier;
        m_notifierRegisterList.append(notData); //
        connect(notifier, &NotificationValue::risingEdge, this, &PCBServer::onNotifierChanged);
    }
}

void PCBServer::sendNotificationToClient(QString message, QByteArray clientID, VeinTcp::TcpPeer *netPeer)
{
    if (clientID.isEmpty()) { // old style communication
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (qint32)0;
        out << message.toUtf8();
        out.device()->seek(0);
        out << (qint32)(block.size() - sizeof(qint32));
        netPeer->writeRaw(block);
    }
    else {
        ProtobufMessage::NetMessage protobufIntMessage;
        ProtobufMessage::NetMessage::NetReply *intMessage = protobufIntMessage.mutable_reply();
        intMessage->set_body(message.toStdString());
        intMessage->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);
        protobufIntMessage.set_clientid(clientID, clientID.count());
        protobufIntMessage.set_messagenr(0); // interrupt
        netPeer->sendMessage(m_protobufWrapper.protobufToByteArray(protobufIntMessage));
    }
}


void PCBServer::onNotifierChanged(quint32 irqreg)
{
    NotificationValue* notifier = qobject_cast<NotificationValue*>(sender());
    if (m_notifierRegisterList.count() > 0) {
        if (m_notifierRegisterList.count() > 0)
            for (int i = 0; i < m_notifierRegisterList.count(); i++) {
                NotificationStructWithValue notData = m_notifierRegisterList.at(i);
                if (notData.notValue == notifier) {
                    QString notificationMsg = QString("IRQ:%1").arg(irqreg);
                    sendNotificationToClient(notificationMsg, notData.clientID, notData.netPeer);
                }
            }
    }
}

void PCBServer::onNotifyPeerConnectionClosed(VeinTcp::TcpPeer *peer)
{
    doUnregisterNotifier(peer);
}

void PCBServer::initSCPIConnections()
{
    for (int i = 0; i < scpiConnectionList.count(); i++)
    {
        scpiConnectionList.at(i)->initSCPIConnection(""); // we have our interface
        connect(scpiConnectionList.at(i), &ScpiConnection::valNotifier, this, &PCBServer::onEstablishNewNotifier);
        connect(scpiConnectionList.at(i), &ScpiConnection::sigNotifySubcriber, this, &PCBServer::onNotifySubscriber);
        connect(scpiConnectionList.at(i), &ScpiConnection::cmdExecutionDone, this, &PCBServer::sendAnswerProto);
        connect(this, &PCBServer::notifierRegistred, scpiConnectionList.at(i), &ScpiConnection::onNotifierRegistered);
        connect(this, &PCBServer::removeSubscribers, scpiConnectionList.at(i), &ScpiConnection::onRemoveSubscribers);
    }
}
