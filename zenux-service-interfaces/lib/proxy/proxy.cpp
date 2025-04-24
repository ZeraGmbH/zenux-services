#include "proxy.h"
#include "proxy_p.h"

namespace Zera {

Proxy* Proxy::getInstance()
{
    if(ProxyPrivate::m_singletonInstance == nullptr)
        ProxyPrivate::m_singletonInstance = new Proxy;
    return ProxyPrivate::m_singletonInstance;
}

ProxyClient* Proxy::getConnection(const QString &ipadress, quint16 port,
                                  VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory)
{
    Q_D(Proxy);
    return d->getConnection(ipadress, port, tcpNetworkFactory);
}

ProxyClient *Proxy::getConnection(const NetworkConnectionInfo &netInfo,
                                  VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory)
{
    Q_D(Proxy);
    return d->getConnection(netInfo.m_sIP, netInfo.m_nPort, tcpNetworkFactory);
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

void Proxy::startConnection(ProxyClient *client)
{
    Q_D(Proxy);
    d->startConnection((ProxyClientPrivate*)client);
}

void Proxy::startConnectionSmart(ProxyClientPtr client)
{
    Q_D(Proxy);
    d->startConnection((ProxyClientPrivate*)(client.get()));
}

bool Proxy::releaseConnection(ProxyClient *client)
{
    Q_D(Proxy);
    return d->releaseConnection((ProxyClientPrivate*) client);
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
