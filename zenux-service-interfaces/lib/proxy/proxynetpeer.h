#ifndef PROXYNETPEER_H
#define PROXYNETPEER_H

#include <vtcp_peer.h>

namespace Zera {

class ProxyNetPeer: public VeinTcp::TcpPeer
{
    Q_OBJECT
public:
    ProxyNetPeer(QObject *qObjParent = 0);
    void startProxyConnection(QString ipAddress, quint16 port);
    bool isStarted();
private:
    bool m_bStarted;
};

}
#endif // PROXYNETPEER_H
