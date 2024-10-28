#ifndef PROXYNETPEER_H
#define PROXYNETPEER_H

#include <vtcp_peer.h>

namespace Zera {

class ProxyNetPeer: public VeinTcp::TcpPeer
{
    Q_OBJECT
public:
    ProxyNetPeer(VeinTcp::AbstractTcpWorkerFactoryPtr tcpNetworkFactory, QObject *qObjParent = nullptr);
    void startProxyConnection(QString ipAddress, quint16 port);
    bool isStarted();
private:
    bool m_bStarted;
};

}
#endif // PROXYNETPEER_H
