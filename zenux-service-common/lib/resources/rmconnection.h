#ifndef RMCONNECTION_H
#define RMCONNECTION_H

#include <xiqnetwrapper.h>
#include <vtcp_peer.h>
#include <netmessages.pb.h>
#include <QString>
#include <QAbstractSocket>

class RMConnection: public QObject
{
    Q_OBJECT
public:
    RMConnection(const QString &ipadr, quint16 port, VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory);
    virtual ~RMConnection();
    void connect2RM();
    void SendIdent(const QString &ident);
    void SendCommand(const QString &cmd, const QString &par, quint32 msgnr);
    void SendCommand(const QString &cmd, const QString &par);
private slots:
    void tcpErrorHandler(VeinTcp::TcpPeer *peer, QAbstractSocket::SocketError errorCode);
    void onMessageReceived(VeinTcp::TcpPeer *peer, QByteArray message);
private:
    void responseHandler(VeinTcp::TcpPeer *peer, std::shared_ptr<google::protobuf::Message> response);
    QString m_sIPAdr;
    quint16 m_nPort;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    QString m_sCommand;
    VeinTcp::TcpPeer* m_pResourceManagerClient = nullptr;
    XiQNetWrapper m_protobufWrapper;
signals:
    void connectionRMError();
    void connected();
    void rmAck(quint32);
};

#endif // RMCONNECTION_H
