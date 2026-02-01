#include "rmconnection.h"

RMConnection::RMConnection(const QString &ipadr, quint16 port, VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory) :
    m_sIPAdr(ipadr),
    m_nPort(port),
    m_tcpFactory(tcpFactory)
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
    m_pResourceManagerClient = new VeinTcp::TcpPeer(m_tcpFactory, this);
    connect(m_pResourceManagerClient, &VeinTcp::TcpPeer::sigSocketError, this, &RMConnection::tcpErrorHandler);
    connect(m_pResourceManagerClient, &VeinTcp::TcpPeer::sigConnectionEstablished, this, &RMConnection::connected);
    connect(m_pResourceManagerClient, &VeinTcp::TcpPeer::sigConnectionClosed, this, &RMConnection::connectionRMError);
    connect(m_pResourceManagerClient, &VeinTcp::TcpPeer::sigMessageReceived, this, &RMConnection::onMessageReceived);
    m_pResourceManagerClient->startConnection(m_sIPAdr, m_nPort);
}

void RMConnection::SendCommand(const QString &cmd, const QString &par, quint32 msgnr)
{
    m_sCommand = cmd;
    ProtobufMessage::NetMessage message;
    ProtobufMessage::NetMessage::ScpiCommand* scpiCmd = message.mutable_scpi();
    scpiCmd->set_command(cmd.toStdString());
    scpiCmd->set_parameter(par.toStdString());
    message.set_messagenr(msgnr);
    m_pResourceManagerClient->sendMessage(m_protobufWrapper.protobufToByteArray(message));
}

void RMConnection::SendCommand(const QString &cmd, const QString &par)
{
    m_sCommand = cmd;
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();
    message->set_command(cmd.toStdString());
    message->set_parameter(par.toStdString());
    m_pResourceManagerClient->sendMessage(m_protobufWrapper.protobufToByteArray(envelope));
}

void RMConnection::tcpErrorHandler(VeinTcp::TcpPeer *peer, QAbstractSocket::SocketError errorCode)
{
    Q_UNUSED(peer)
    qCritical("tcp socket error resource manager port: %d", errorCode);
    emit connectionRMError();
}

void RMConnection::onMessageReceived(VeinTcp::TcpPeer *peer, const QByteArray &message)
{
    responseHandler(peer, m_protobufWrapper.byteArrayToProtobuf(message));
}

void RMConnection::responseHandler(VeinTcp::TcpPeer *peer, std::shared_ptr<google::protobuf::Message> response)
{
    Q_UNUSED(peer)
    std::shared_ptr<ProtobufMessage::NetMessage> answer = std::static_pointer_cast<ProtobufMessage::NetMessage>(response);
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

void RMConnection::SendIdent(const QString &ident)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::NetReply* message = envelope.mutable_reply();
    message->set_rtype(ProtobufMessage::NetMessage::NetReply::IDENT);
    message->set_body(ident.toStdString());
    m_pResourceManagerClient->sendMessage(m_protobufWrapper.protobufToByteArray(envelope));
}
