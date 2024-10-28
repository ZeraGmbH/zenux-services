#include "proxynetpeer.h"

namespace Zera {

ProxyNetPeer::ProxyNetPeer(VeinTcp::AbstractTcpWorkerFactoryPtr tcpWorkerFactory, QObject *qObjParent) :
    VeinTcp::TcpPeer(tcpWorkerFactory, qObjParent)
{
    m_bStarted = false;
}

void ProxyNetPeer::startProxyConnection(QString ipAddress, quint16 port)
{
    startConnection(ipAddress, port);
    m_bStarted = true;
}

bool ProxyNetPeer::isStarted()
{
    return m_bStarted;
}

}
