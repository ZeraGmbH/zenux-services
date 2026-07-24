#ifndef SCPISINGLETRANSACTIONBLOCKED_H
#define SCPISINGLETRANSACTIONBLOCKED_H

#include "netmessages.pb.h"
#include "proxyclient.h"
#include <mocktcpnetworkfactory.h>
#include <QString>

class ScpiSingleTransactionBlocked
{
public:
    // 6307: com5003d/mt310s2 as default
    static QString query(const QString &scpiQuery,
                         quint16 port = 6307,
                         const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create(),
                         const Zera::ProxyClientPtr &proxyClient = nullptr);
    static QString cmd(const QString &scpiCmd, const QString &param,
                       quint16 port = 6307,
                       const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create(),
                       const Zera::ProxyClientPtr &proxyClient = nullptr);
    static QString cmdXmlParam(const QString &scpiCmd, const QString &param,
                               quint16 port = 6307,
                               const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create(),
                               const Zera::ProxyClientPtr &proxyClient = nullptr);
private:
    static QString sendBlocked(ProtobufMessage::NetMessage &envelope,
                               quint16 port,
                               const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory,
                               Zera::ProxyClientPtr proxyClient);
};

#endif // SCPISINGLETRANSACTIONBLOCKED_H
