#ifndef SCPISINGLETRANSACTIONBLOCKED_H
#define SCPISINGLETRANSACTIONBLOCKED_H

#include "netmessages.pb.h"
#include "proxyclient.h"
#include <QString>

class ScpiSingleTransactionBlocked
{
public:
    // 6307: com5003d/mt310s2 as default
    static QString query(QString scpiQuery, quint16 port = 6307, Zera::ProxyClientPtr proxyClient = nullptr);
    static QString cmd(QString scpiCmd, QString param, quint16 port = 6307, Zera::ProxyClientPtr proxyClient = nullptr);
    static QString cmdXmlParam(QString scpiCmd, QString param, quint16 port = 6307, Zera::ProxyClientPtr proxyClient = nullptr);
private:
    static QString sendBlocked(ProtobufMessage::NetMessage &envelope, quint16 port, Zera::ProxyClientPtr proxyClient);
};

#endif // SCPISINGLETRANSACTIONBLOCKED_H
