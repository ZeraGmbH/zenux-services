#ifndef PROXY_P_H
#define PROXY_P_H

#include "proxy.h"
#include <vtcp_peer.h>
#include <xiqnetwrapper.h>
#include <QHash>
#include <QString>

namespace Zera {

class Proxy;
class ProxyClient;
class ProxyClientPrivate;
class ProxyConnection;
class ProxyNetPeer;

class ProxyPrivate: public QObject
{
    Q_OBJECT
public:
    quint32 transmitCommand(ProxyClientPrivate *client, ProtobufMessage::NetMessage *message);

private slots:
    void receiveTcpError(VeinTcp::TcpPeer *peer, QAbstractSocket::SocketError errorCode);
    void registerConnection(VeinTcp::TcpPeer *peer);
    void registerDisConnection(VeinTcp::TcpPeer *peer);
    void onMessageReceived(VeinTcp::TcpPeer *peer, const QByteArray &message);
private:
    ProxyPrivate(Proxy *parent);
    ProxyClientPtr getConnectionSmart(QString ipadress, quint16 port,
                                      VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory);
    void startConnection(ProxyClientPrivate *client); // TODO replace by smart variant
    bool releaseConnection(ProxyClientPrivate *client); // TODO replace by smart variant
    void handleReceiveMessage(std::shared_ptr<google::protobuf::Message> message);
    ProxyNetPeer *getProxyNetPeer(const QString &ipadress, quint16 port,
                                  VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory);
    ProxyNetPeer *searchConnection(const QString &ip, quint16 port);

    static Proxy* m_singletonInstance;
    Proxy *q_ptr;
    XiQNetWrapper m_protobufWrapper;
    QHash<ProxyClientPrivate*, ProxyConnection*> m_ConnectionHash; // TODO replace by smart variant
    QHash<QByteArray, ProxyClientPrivate*> m_ClientHash;

    Q_DISABLE_COPY(ProxyPrivate)
    Q_DECLARE_PUBLIC(Proxy)
};

}

#endif // PROXY_P_H
