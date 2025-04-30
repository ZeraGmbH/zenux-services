#ifndef PROXY_H
#define PROXY_H

#include "service-interfaces_export.h"
#include "proxyclient.h"
#include "networkconnectioninfo.h"
#include <abstracttcpnetworkfactory.h>

namespace Zera {
class ProxyPrivate;

class SERVICE_INTERFACES_EXPORT Proxy : public QObject
{
    Q_OBJECT
public:
    static Proxy* getInstance();
    ProxyClientPtr getConnectionSmart(const QString &ipadress, quint16 port,
                                      VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory);
    ProxyClientPtr getConnectionSmart(const NetworkConnectionInfo &netInfo,
                                      VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory);
    void startConnectionSmart(ProxyClientPtr client);
    bool releaseConnection(ProxyClient* client);
    bool releaseConnectionSmart(ProxyClientPtr client);

protected:
    explicit Proxy();
    ~Proxy();
private:
    ProxyPrivate *d_ptr;

    Q_DISABLE_COPY(Proxy)
    Q_DECLARE_PRIVATE(Proxy)
};

}

#endif // PROXY_H
