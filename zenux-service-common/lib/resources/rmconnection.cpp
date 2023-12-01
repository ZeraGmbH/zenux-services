#include "rmconnection.h"

RMConnection::RMConnection(QString ipadr, quint16 port) :
    m_sIPAdr(ipadr),
    m_nPort(port)
{
}

RMConnection::~RMConnection()
{
    delete m_pResourceManagerClient;
}

void RMConnection::connect2RM()
{
    if (m_pResourceManagerClient) {
        delete m_pResourceManagerClient;
        qCritical("RMConnection::connect2RM called with connection open!");
    }
    m_pResourceManagerClient = new XiQNetPeer(this);
    m_pResourceManagerClient->setWrapper(&m_ProtobufWrapper);
    connect(m_pResourceManagerClient, &XiQNetPeer::sigSocketError, this, &RMConnection::tcpErrorHandler);
    connect(m_pResourceManagerClient, &XiQNetPeer::sigConnectionEstablished, this, &RMConnection::connected);
    connect(m_pResourceManagerClient, &XiQNetPeer::sigConnectionClosed, this, &RMConnection::connectionRMError);
    connect(m_pResourceManagerClient, &XiQNetPeer::sigMessageReceived, this, &RMConnection::responseHandler);
    m_pResourceManagerClient->startConnection(m_sIPAdr, m_nPort);
}

void RMConnection::SendCommand(QString &cmd, QString &par, quint32 msgnr)
{
    m_sCommand = cmd;
    ProtobufMessage::NetMessage message;
    ProtobufMessage::NetMessage::ScpiCommand* scpiCmd = message.mutable_scpi();
    scpiCmd->set_command(cmd.toStdString());
    scpiCmd->set_parameter(par.toStdString());
    message.set_messagenr(msgnr);
    m_pResourceManagerClient->sendMessage(message);
}

void RMConnection::SendCommand(QString &cmd, QString &par)
{
    m_sCommand = cmd;
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();
    message->set_command(cmd.toStdString());
    message->set_parameter(par.toStdString());
    m_pResourceManagerClient->sendMessage(envelope);
}

void RMConnection::tcpErrorHandler(QAbstractSocket::SocketError errorCode)
{
    qCritical("tcp socket error resource manager port: %d",errorCode);
    emit connectionRMError();
}

void RMConnection::responseHandler(std::shared_ptr<google::protobuf::Message> response)
{
    std::shared_ptr<ProtobufMessage::NetMessage> answer = nullptr;
    answer = std::static_pointer_cast<ProtobufMessage::NetMessage>(response);
    if (answer != nullptr) {
        if ( !(answer->has_reply() && answer->reply().rtype() == answer->reply().ACK)) {
            qWarning("command: %s, was not acknowledged", qPrintable(m_sCommand));
            emit connectionRMError();
        }
        else {
            emit rmAck(answer->messagenr());
        }
    }
    else {
        qCritical("answer from resource manager not protobuf");
        emit connectionRMError();
    }
}

void RMConnection::SendIdent(QString ident)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::NetReply* message = envelope.mutable_reply();
    message->set_rtype(ProtobufMessage::NetMessage::NetReply::IDENT);
    message->set_body(ident.toStdString());
    m_pResourceManagerClient->sendMessage(envelope);
}
