#include "pcbserver.h"
#include "protonetcommand.h"
#include <xiqnetpeer.h>
#include <xmlconfigreader.h>
#include <xiqnetserver.h>
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

cPCBServer::cPCBServer(ServerParams params, cSCPI *scpiInterface) :
    ScpiConnection(scpiInterface),
    m_ethSettings(&m_xmlConfigReader),
    m_params(params)
{
}

void cPCBServer::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SERVER").arg(leadingNodes), "REGISTER", SCPI::isCmdwP, m_pSCPIInterface, cmdRegister);
    addDelegate(QString("%1SERVER").arg(leadingNodes), "UNREGISTER", SCPI::isQuery | SCPI::isCmd, m_pSCPIInterface, cmdUnregister);
}

cSCPI *cPCBServer::getSCPIInterface()
{
    return m_pSCPIInterface;
}

QString &cPCBServer::getName()
{
    return m_params.name;
}

QString &cPCBServer::getVersion()
{
    return m_params.version;
}

void cPCBServer::setupServer()
{
    myServer = new XiQNetServer(this); // our working (talking) horse
    myServer->setDefaultWrapper(&m_ProtobufWrapper);
    connect(myServer,&XiQNetServer::sigClientConnected,this,&cPCBServer::onEstablishNewConnection);
    if(m_ethSettings.isSCPIactive()) {
        m_pSCPIServer = new QTcpServer();
        m_pSCPIServer->setMaxPendingConnections(1); // we only accept 1 client to connect
        connect(m_pSCPIServer, &QTcpServer::newConnection, this, &cPCBServer::setSCPIConnection);
    }
}

void cPCBServer::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
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

void cPCBServer::sendAnswerProto(cProtonetCommand *protoCmd)
{
    if(protoCmd->m_pPeer == 0) {
        // we worked on a command comming from scpi socket connection
        QString answer = protoCmd->m_sOutput+"\n";
        QByteArray ba = answer.toLatin1();
        m_pSCPISocket->write(ba);
    }
    else {
        if(protoCmd->m_bhasClientId) {
            ProtobufMessage::NetMessage protobufAnswer;
            ProtobufMessage::NetMessage::NetReply *Answer = protobufAnswer.mutable_reply();
            // dependent on rtype caller can see ack, nak, error
            // in case of error the body has to be analyzed for details
            QString output = protoCmd->m_sOutput;
            if (output.contains(SCPI::scpiAnswer[SCPI::ack]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);
            else if (output.contains(SCPI::scpiAnswer[SCPI::nak]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_NACK);
            else if (output.contains(SCPI::scpiAnswer[SCPI::busy]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(SCPI::scpiAnswer[SCPI::errcon]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(SCPI::scpiAnswer[SCPI::erraut]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(SCPI::scpiAnswer[SCPI::errval]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(SCPI::scpiAnswer[SCPI::errxml]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(SCPI::scpiAnswer[SCPI::errmmem]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(SCPI::scpiAnswer[SCPI::errpath]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(SCPI::scpiAnswer[SCPI::errexec]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(SCPI::scpiAnswer[SCPI::errtimo]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);

            Answer->set_body(output.toStdString()); // in any case we set the body
            protobufAnswer.set_clientid(protoCmd->m_clientId, protoCmd->m_clientId.count());
            protobufAnswer.set_messagenr(protoCmd->m_nmessageNr);
            protoCmd->m_pPeer->sendMessage(protobufAnswer);
        }
        else {
            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_0);
            out << (qint32)0;

            out << protoCmd->m_sOutput.toUtf8();
            out.device()->seek(0);
            out << (qint32)(block.size() - sizeof(qint32));

            protoCmd->m_pPeer->getTcpSocket()->write(block);
        }
    }
    delete protoCmd;
}

void cPCBServer::setSCPIConnection()
{
    m_pSCPISocket = m_pSCPIServer->nextPendingConnection();
    connect(m_pSCPISocket, &QIODevice::readyRead, this, &cPCBServer::SCPIInput);
    connect(m_pSCPISocket, &QAbstractSocket::disconnected, this, &cPCBServer::SCPIdisconnect);
}

void cPCBServer::SCPIInput()
{
    m_sInput = "";
    while(m_pSCPISocket->canReadLine())
        m_sInput += m_pSCPISocket->readLine();
    m_sInput.remove('\r'); // we remove cr lf
    m_sInput.remove('\n');

    QByteArray clientId = QByteArray(); // we set an empty byte array
    cProtonetCommand* protoCmd = new cProtonetCommand(0, false, true, clientId, 0, m_sInput);
    // peer = 0 means we are working on the scpi socket ....
    cSCPIObject* scpiObject = m_pSCPIInterface->getSCPIObject(m_sInput);
    if(scpiObject) {
        cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
        if (!scpiDelegate->executeSCPI(protoCmd)) {
            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
            emit cmdExecutionDone(protoCmd);
        }
    }
    else {
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
        emit cmdExecutionDone(protoCmd);
    }
}

void cPCBServer::SCPIdisconnect()
{
    disconnect(m_pSCPISocket, 0, 0, 0); // we disconnect everything
}

void cPCBServer::onSendNotification(ScpiNotificationSubscriber subscriber)
{
    QString notificationMsg = QString("Notify:%1").arg(subscriber.m_notifierId);
    sendNotificationToClient(notificationMsg, subscriber.m_clientId, subscriber.m_netPeer);
}

void cPCBServer::registerNotifier(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if(cmd.isCommand(1)) { // SEC service receives registerNotifier with 1 param
        QString query = cmd.getParam(0);
        cSCPIObject* scpiObject = m_pSCPIInterface->getSCPIObject(query);
        if(scpiObject) {
            NotificationStructWithValue notData;
            notData.netPeer = protoCmd->m_pPeer;
            notData.clientID = protoCmd->m_clientId;
            connect(notData.netPeer, &XiQNetPeer::sigConnectionClosed, this, &cPCBServer::onNotifyPeerConnectionClosed);
            m_notifierRegisterNext.append(notData); // we wait for a notifier signal

            cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
            cProtonetCommand* procmd = new cProtonetCommand(protoCmd);
            procmd->m_nSCPIType = SCPI::isQuery; // we need to set query type for proper execution
            procmd->m_bwithOutput = false;
            procmd->m_sInput = query;
            if (!scpiDelegate->executeSCPI(procmd)) {
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
                m_notifierRegisterNext.pop_back();
            }
            else
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack]; // we overwrite the query's output here
        }
        else
            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
    }
    else if(cmd.isCommand(2)) {// mt310s2/com5003 service receives registerNotifier with 2 params
        QString query = cmd.getParam(0);
        cSCPIObject* scpiObject = m_pSCPIInterface->getSCPIObject(query);
        if(scpiObject) {
            connect( protoCmd->m_pPeer, &XiQNetPeer::sigConnectionClosed, this, &cPCBServer::onNotifyPeerConnectionClosed);
            cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
            cProtonetCommand* procmd = new cProtonetCommand(protoCmd);
            procmd->m_bwithOutput = false;
            procmd->m_sInput = query;
            ScpiNotificationSubscriber subscriber(protoCmd->m_pPeer, protoCmd->m_clientId, cmd.getParam(1).toInt());
            scpiDelegate->getScpiNotificationSubscriberHandler().addSubscriber(subscriber);
            scpiDelegate->executeSCPI(procmd);
            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack]; // we overwrite the query's output here
            emit notifierRegistred(scpiDelegate->getNotificationString());
        }
        else
            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
    }
}

void cPCBServer::unregisterNotifier(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if(cmd.isCommand(1) && (cmd.getParam(0) == "") ) {
        doUnregisterNotifier(protoCmd->m_pPeer, protoCmd->m_clientId);
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack];
    }
    else
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
}

void cPCBServer::doUnregisterNotifier(XiQNetPeer* peer, const QByteArray &clientID)
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

void cPCBServer::onEstablishNewConnection(XiQNetPeer *newClient)
{
    connect(newClient, &XiQNetPeer::sigMessageReceived, this, &cPCBServer::onExecuteCommandProto);
}

void cPCBServer::onExecuteCommandProto(std::shared_ptr<google::protobuf::Message> cmd)
{
    cSCPIObject* scpiObject;
    XiQNetPeer* peer = qobject_cast<XiQNetPeer*>(sender());
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
                m_sInput = QString::fromStdString(scpiCmd.command()) +  " " + QString::fromStdString(scpiCmd.parameter());
                cProtonetCommand* protoCmd;
                scpiObject =  m_pSCPIInterface->getSCPIObject(m_sInput);
                if (scpiObject) {
                    protoCmd = new cProtonetCommand(peer, true, true, clientId, messageNr, m_sInput, scpiObject->getType());
                    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
                    if (!scpiDelegate->executeSCPI(protoCmd)) {
                        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
                        emit cmdExecutionDone(protoCmd);
                    }
                }
                else {
                    protoCmd = new cProtonetCommand(peer, true, true, clientId, messageNr, m_sInput);
                    protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
                    emit cmdExecutionDone(protoCmd);
                }
                // we get a signal when a command is finished and send answer then
            }
        }
        else {
            m_sInput =  QString::fromStdString(protobufCommand->scpi().command());
            QByteArray clientId = QByteArray(); // we set an empty byte array
            cProtonetCommand* protoCmd;
            scpiObject =  m_pSCPIInterface->getSCPIObject(m_sInput);
            if (scpiObject) {
                protoCmd = new cProtonetCommand(peer, false, true, clientId, 0, m_sInput, scpiObject->getType());
                cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
                if (!scpiDelegate->executeSCPI(protoCmd)) {
                    protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
                    emit cmdExecutionDone(protoCmd);
                }
            }
            else {
                protoCmd = new cProtonetCommand(peer, false, true, clientId, 0, m_sInput, 0);
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
                emit cmdExecutionDone(protoCmd);
            }
        }
    }
}

void cPCBServer::onEstablishNewNotifier(NotificationValue *notifier)
{
    if (m_notifierRegisterNext.count() > 0) // if we're waiting for notifier
    {
        disconnect(notifier, 0, 0, 0); // we disconnect first because we only want 1 signal
        NotificationStructWithValue notData = m_notifierRegisterNext.takeFirst(); // we pick the notification data
        notData.notValue = notifier;
        m_notifierRegisterList.append(notData); //
        connect(notifier, &NotificationValue::risingEdge, this, &cPCBServer::onNotifierChanged);
    }
}

void cPCBServer::sendNotificationToClient(QString message, QByteArray clientID, XiQNetPeer *netPeer)
{
    if (clientID.isEmpty()) { // old style communication
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (qint32)0;
        out << message.toUtf8();
        out.device()->seek(0);
        out << (qint32)(block.size() - sizeof(qint32));
        netPeer->getTcpSocket()->write(block);
    }
    else {
        ProtobufMessage::NetMessage protobufIntMessage;
        ProtobufMessage::NetMessage::NetReply *intMessage = protobufIntMessage.mutable_reply();
        intMessage->set_body(message.toStdString());
        intMessage->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);
        protobufIntMessage.set_clientid(clientID, clientID.count());
        protobufIntMessage.set_messagenr(0); // interrupt
        netPeer->sendMessage(protobufIntMessage);
    }
}

void cPCBServer::onNotifierChanged(quint32 irqreg)
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

void cPCBServer::onNotifyPeerConnectionClosed()
{
    XiQNetPeer *peer = qobject_cast<XiQNetPeer*>(QObject::sender());
    doUnregisterNotifier(peer);
}

void cPCBServer::initSCPIConnections()
{
    for (int i = 0; i < scpiConnectionList.count(); i++)
    {
        scpiConnectionList.at(i)->initSCPIConnection(""); // we have our interface
        connect(scpiConnectionList.at(i), &ScpiConnection::valNotifier, this, &cPCBServer::onEstablishNewNotifier);
        connect(scpiConnectionList.at(i), &ScpiConnection::sendNotification, this, &cPCBServer::onSendNotification);
        connect(scpiConnectionList.at(i), &ScpiConnection::cmdExecutionDone, this, &cPCBServer::sendAnswerProto);
        connect(this, &cPCBServer::notifierRegistred, scpiConnectionList.at(i), &ScpiConnection::onNotifierRegistered);
        connect(this, &cPCBServer::removeSubscribers, scpiConnectionList.at(i), &ScpiConnection::onRemoveSubscribers);
    }
}
