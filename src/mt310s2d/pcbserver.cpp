#include <protonetcommand.h>
#include "resource.h"
#include <scpiconnection.h>
#include "pcbserver.h"
#include "mt310s2dglobal.h"
#include "settings/ethsettings.h"
#include <xiqnetpeer.h>
#include <xmlconfigreader.h>
#include <xiqnetserver.h>
#include <scpi.h>
#include <scpisingletonfactory.h>
#include <fcntl.h>
#include <unistd.h>
#include <netmessages.pb.h>
#include <QtDebug>
#include <QFile>
#include <QByteArray>
#include <QList>
#include <QString>
#include <QTcpSocket>
#include <QTcpServer>

cPCBServer::cPCBServer()
    : cSCPIConnection(ScpiSingletonFactory::getScpiObj(ServerName))
{
    m_nMsgNr = 0;
    m_sServerName = ServerName;
    m_sServerVersion = ServerVersion;
    myXMLConfigReader = new Zera::XMLConfig::cReader();
}


void cPCBServer::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1SERVER").arg(leadingNodes),"REGISTER",SCPI::isCmdwP, m_pSCPIInterface, PCBServer::cmdRegister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SERVER").arg(leadingNodes),"UNREGISTER",SCPI::isQuery | SCPI::isCmd , m_pSCPIInterface, PCBServer::cmdUnregister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));

}


cSCPI *cPCBServer::getSCPIInterface()
{
    return m_pSCPIInterface;
}


quint32 cPCBServer::getMsgNr()
{
    m_nMsgNr++;
    if (m_nMsgNr == 0)
        m_nMsgNr++;
    return m_nMsgNr;
}


QString &cPCBServer::getName()
{
    return m_sServerName;
}


QString &cPCBServer::getVersion()
{
    return m_sServerVersion;
}


void cPCBServer::setupServer()
{
    myServer = new XiQNetServer(this); // our working (talking) horse
    myServer->setDefaultWrapper(&m_ProtobufWrapper);
    connect(myServer,SIGNAL(sigClientConnected(XiQNetPeer*)),this,SLOT(establishNewConnection(XiQNetPeer*)));

    if (m_pETHSettings->isSCPIactive())
    {
        m_pSCPIServer = new QTcpServer();
        m_pSCPIServer->setMaxPendingConnections(1); // we only accept 1 client to connect
        connect(m_pSCPIServer, SIGNAL(newConnection()), this, SLOT(setSCPIConnection()));
    }

}


void cPCBServer::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case PCBServer::cmdRegister:
        m_RegisterNotifier(protoCmd);
        break;
    case PCBServer::cmdUnregister:
        m_UnregisterNotifier(protoCmd);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


void cPCBServer::sendAnswer(cProtonetCommand *protoCmd)
{
    if (protoCmd->m_pPeer == 0)
    {
        // we worked on a command comming from scpi socket connection
        // todo: clean up this mess
        QByteArray ba;
        QString answer;

        answer = protoCmd->m_sOutput+"\n";
        ba = answer.toLatin1();
        m_pSCPISocket->write(ba);
    }
    else
    {
        if (protoCmd->m_bhasClientId)
        {
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

        else

        {
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
    connect(m_pSCPISocket, SIGNAL(readyRead()), this, SLOT(SCPIInput()));
    connect(m_pSCPISocket, SIGNAL(disconnected()), this, SLOT(SCPIdisconnect()));
}


void cPCBServer::SCPIInput()
{
    QString m_sInput;
    QString dummy;
    cSCPIObject* scpiObject;

    m_sInput = "";
    while ( m_pSCPISocket->canReadLine() )
        m_sInput += m_pSCPISocket->readLine();

    m_sInput.remove('\r'); // we remove cr lf
    m_sInput.remove('\n');

    QByteArray clientId = QByteArray(); // we set an empty byte array
    cProtonetCommand* protoCmd = new cProtonetCommand(0, false, true, clientId, 0, m_sInput);
    // peer = 0 means we are working on the scpi socket ....

    if ( (scpiObject =  m_pSCPIInterface->getSCPIObject(m_sInput, dummy)) != 0)
    {
        cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
        if (!scpiDelegate->executeSCPI(protoCmd))
        {
            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
            emit cmdExecutionDone(protoCmd);
        }
    }
    else
    {
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
        emit cmdExecutionDone(protoCmd);
    }
}


void cPCBServer::SCPIdisconnect()
{
    disconnect(m_pSCPISocket, 0, 0, 0); // we disconnect everything
}


void cPCBServer::m_RegisterNotifier(cProtonetCommand *protoCmd)
{
    bool ok;
    QString dummy;
    cSCPICommand cmd = protoCmd->m_sInput;

    if (cmd.isCommand(2))
    {
        cSCPIObject* scpiObject;
        QString query = cmd.getParam(0);

        if ( (scpiObject =  m_pSCPIInterface->getSCPIObject(query, dummy)) != 0)
        {
            cNotificationData notData;

            notData.netPeer = protoCmd->m_pPeer;
            notData.clientID = protoCmd->m_clientId;
            notData.notifier = cmd.getParam(1).toInt(&ok);
            connect(notData.netPeer, &XiQNetPeer::sigConnectionClosed, this, &cPCBServer::peerConnectionClosed);

            notifierRegisterNext.append(notData); // we wait for a notifier signal

            cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
            cProtonetCommand* procmd = new cProtonetCommand(protoCmd);
            procmd->m_bwithOutput = false;
            procmd->m_sInput = query;

            if (!scpiDelegate->executeSCPI(procmd))
            {
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
                notifierRegisterNext.pop_back();
            }
            else
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack]; // we overwrite the query's output here
        }
        else
            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];

    }
}


void cPCBServer::m_UnregisterNotifier(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == "") )
    {
        doUnregisterNotifier(protoCmd->m_pPeer, protoCmd->m_clientId);
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack];
    }
    else
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
}


void cPCBServer::doUnregisterNotifier(XiQNetPeer* peer, const QByteArray &clientID)
{
    if (notifierRegisterList.count() > 0) {
        // we have to remove all notifiers for this client and or clientId
        // iterate backwards so removals do not confuse our loop
        for (int i = notifierRegisterList.count()-1; i >= 0; i--) {
            cNotificationData notData = notifierRegisterList.at(i);
            if (peer == notData.netPeer) {
                // we found the client
                if (clientID.isEmpty() || notData.clientID.isEmpty() || (notData.clientID == clientID)) {
                    notifierRegisterList.removeAt(i);
                }
            }
        }
    }
}

void cPCBServer::establishNewConnection(XiQNetPeer *newClient)
{
    connect(newClient, &XiQNetPeer::sigMessageReceived ,this, QOverload<std::shared_ptr<google::protobuf::Message> >::of(&cPCBServer::executeCommand));
    // later ... connect(newClient,SIGNAL(sigMessageReceived(QByteArray*)),this,SLOT(executeCommand(QByteArray*)));
}


void cPCBServer::executeCommand(std::shared_ptr<google::protobuf::Message> cmd)
{
    std::shared_ptr<ProtobufMessage::NetMessage> protobufCommand = nullptr;
    cSCPIObject* scpiObject;
    QString dummy;

    //XiQNetPeer* client = qobject_cast<XiQNetPeer*>(sender());

    XiQNetPeer* peer = qobject_cast<XiQNetPeer*>(sender());
    protobufCommand = std::static_pointer_cast<ProtobufMessage::NetMessage>(cmd);

    if ( (protobufCommand != nullptr) && (peer != nullptr))
    {
        if (protobufCommand->has_clientid())
        {
            QByteArray clientId = QByteArray(protobufCommand->clientid().data(), protobufCommand->clientid().size());

            if (protobufCommand->has_netcommand())
            {
                // in case of "lost" clients we delete registration for notification
                doUnregisterNotifier(peer, clientId);
            }

            else

            if (protobufCommand->has_messagenr())
            {
                quint32 messageNr = protobufCommand->messagenr();
                ProtobufMessage::NetMessage::ScpiCommand scpiCmd = protobufCommand->scpi();
                m_sInput = QString::fromStdString(scpiCmd.command()) +  " " + QString::fromStdString(scpiCmd.parameter());
                cProtonetCommand* protoCmd = new cProtonetCommand(peer, true, true, clientId, messageNr, m_sInput);
                if ( (scpiObject =  m_pSCPIInterface->getSCPIObject(m_sInput, dummy)) != 0)
                {
                    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
                    if (!scpiDelegate->executeSCPI(protoCmd))
                    {
                        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
                        emit cmdExecutionDone(protoCmd);
                    }
                }
                else
                {
                    protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
                    emit cmdExecutionDone(protoCmd);
                }

                // we get a signal when a command is finished and send answer then
            }
        }

        else

        {
            m_sInput =  QString::fromStdString(protobufCommand->scpi().command());
            QByteArray clientId = QByteArray(); // we set an empty byte array
            cProtonetCommand* protoCmd = new cProtonetCommand(peer, false, true, clientId, 0, m_sInput);
            if ( (scpiObject =  m_pSCPIInterface->getSCPIObject(m_sInput, dummy)) != 0)
            {
                cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);

                if (!scpiDelegate->executeSCPI(protoCmd))
                {
                    protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
                    emit cmdExecutionDone(protoCmd);
                }

            }
            else
            {
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
                emit cmdExecutionDone(protoCmd);
            }
        }
    }
}


void cPCBServer::establishNewNotifier(cNotificationString *notifier)
{
    if (notifierRegisterNext.count() > 0) // if we're waiting for notifier
    {
        disconnect(notifier, 0, 0, 0); // we disconnect first because we only want 1 signal
        cNotificationData notData = notifierRegisterNext.takeFirst(); // we pick the notification data
        notData.notString = notifier;
        notifierRegisterList.append(notData); //
        connect(notifier, SIGNAL(valueChanged()), this, SLOT(asyncHandler()));
    }
}


void cPCBServer::asyncHandler()
{
    cNotificationString* notifier = qobject_cast<cNotificationString*>(sender());

    if (notifierRegisterList.count() > 0)
    {
        ProtobufMessage::NetMessage protobufIntMessage;

        if (notifierRegisterList.count() > 0)
            for (int i = 0; i < notifierRegisterList.count(); i++)
            {
                cNotificationData notData = notifierRegisterList.at(i);
                if (notData.notString == notifier)
                {
                    ProtobufMessage::NetMessage::NetReply *intMessage = protobufIntMessage.mutable_reply();
                    QString s = QString("Notify:%1").arg(notData.notifier);
                    if (notData.clientID.isEmpty()) // old style communication
                    {
                        QByteArray block;

                        QDataStream out(&block, QIODevice::WriteOnly);
                        out.setVersion(QDataStream::Qt_4_0);
                        out << (qint32)0;

                        out << s.toUtf8();
                        out.device()->seek(0);
                        out << (qint32)(block.size() - sizeof(qint32));

                        notData.netPeer->getTcpSocket()->write(block);
                    }
                    else
                    {
                        intMessage->set_body(s.toStdString());
                        intMessage->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);
                        QByteArray id = notData.clientID;
                        protobufIntMessage.set_clientid(id, id.count());
                        protobufIntMessage.set_messagenr(0); // interrupt

                        notData.netPeer->sendMessage(protobufIntMessage);
                    }
                }
            }
    }
}


void cPCBServer::peerConnectionClosed()
{
    XiQNetPeer *peer = qobject_cast<XiQNetPeer*>(QObject::sender());
    doUnregisterNotifier(peer);
}


void cPCBServer::initSCPIConnections()
{
    for (int i = 0; i < scpiConnectionList.count(); i++)
    {
        scpiConnectionList.at(i)->initSCPIConnection(""); // we have our interface
        connect(scpiConnectionList.at(i), &cSCPIConnection::strNotifier, this, &cPCBServer::establishNewNotifier);
        connect(scpiConnectionList.at(i), SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SLOT(sendAnswer(cProtonetCommand*)));
    }
}


