#include "proxy.h"
#include "proxy_p.h"

namespace Zera {

Proxy* Proxy::getInstance()
{
    if(ProxyPrivate::m_singletonInstance == nullptr)
        ProxyPrivate::m_singletonInstance = new Proxy;
    return ProxyPrivate::m_singletonInstance;
}

ProxyClientPtr Proxy::getConnectionSmart(const QString &ipadress, quint16 port,
                                         VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory)
{
    Q_D(Proxy);
    return d->getConnectionSmart(ipadress, port, tcpNetworkFactory);
}

ProxyClientPtr Proxy::getConnectionSmart(const NetworkConnectionInfo &netInfo,
                                         VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory)
{
    Q_D(Proxy);
    return d->getConnectionSmart(netInfo.m_sIP, netInfo.m_nPort, tcpNetworkFactory);
}

void Proxy::startConnectionSmart(ProxyClientPtr client)
{
    Q_D(Proxy);
    d->startConnectionSmart(client);
}

bool Proxy::releaseConnection(ProxyClient *client)
{
    Q_D(Proxy);
    return d->releaseConnection((ProxyClientPrivate*) client);
}

bool Proxy::releaseConnectionSmart(ProxyClientPtr client)
{
    Q_D(Proxy);
    return d->releaseConnection((ProxyClientPrivate*) client.get());
}

Proxy::Proxy():
    d_ptr(new Zera::ProxyPrivate(this))
{
}

Proxy::~Proxy()
{
    delete d_ptr;
}

}
