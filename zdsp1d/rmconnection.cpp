#include <QString>
#include <xiqnetpeer.h>
#include <netmessages.pb.h>
#include "rmconnection.h"

cRMConnection::cRMConnection(QString ipadr, quint16 port)
    :m_sIPAdr(ipadr), m_nPort(port)
{
    m_pResourceManagerClient = 0;
}

void cRMConnection::connect2RM()
{
    if (m_pResourceManagerClient)
        delete m_pResourceManagerClient;
    m_pResourceManagerClient = new XiQNetPeer(this);
    m_pResourceManagerClient->setWrapper(&m_ProtobufWrapper);
    connect(m_pResourceManagerClient, &XiQNetPeer::sigSocketError, this, &cRMConnection::tcpErrorHandler);
    connect(m_pResourceManagerClient, &XiQNetPeer::sigConnectionEstablished, this, &cRMConnection::connected);
    connect(m_pResourceManagerClient, &XiQNetPeer::sigConnectionClosed, this, &cRMConnection::connectionRMError);
    connect(m_pResourceManagerClient, &XiQNetPeer::sigMessageReceived, this, &cRMConnection::responseHandler);
    m_pResourceManagerClient->startConnection(m_sIPAdr, m_nPort);
}

void cRMConnection::SendCommand(QString &cmd, QString &par)
{
    m_sCommand = cmd;
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();
    message->set_command(cmd.toStdString());
    message->set_parameter(par.toStdString());
    m_pResourceManagerClient->sendMessage(envelope);
}

void cRMConnection::tcpErrorHandler(QAbstractSocket::SocketError errorCode)
{
    qWarning("tcp socket error resource manager port: %d", errorCode);
    emit connectionRMError();
}

void cRMConnection::responseHandler(std::shared_ptr<google::protobuf::Message> message)
{
    std::shared_ptr<ProtobufMessage::NetMessage> answer = nullptr;
    answer = std::static_pointer_cast<ProtobufMessage::NetMessage>(message);
    if ( !(answer->has_reply() && answer->reply().rtype() == answer->reply().ACK)) {
        qWarning("Error: Command %s  was not acknowledged", qPrintable(m_sCommand));
        emit connectionRMError();
    }
}

void cRMConnection::SendIdent(QString ident)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::NetReply* message = envelope.mutable_reply();
    message->set_rtype(ProtobufMessage::NetMessage::NetReply::IDENT);
    message->set_body(ident.toStdString());

    m_pResourceManagerClient->sendMessage(envelope);
}

