#ifndef PROXYCONNECTION_H
#define PROXYCONNECTION_H

#include "proxynetpeer.h"
#include <QByteArray>

namespace Zera {

struct ProxyConnection
{
    ProxyConnection(const QString &ip, quint16 port, const QByteArray &uuid, ProxyNetPeer* client) :
        m_sIP(ip),
        m_nPort(port),
        m_binUUID(uuid),
        m_pNetClient(client){
    }
    const QString m_sIP;
    const quint16 m_nPort;
    const QByteArray m_binUUID;
    ProxyNetPeer *m_pNetClient;
};

}

#endif // PROXYCONNECTION_H
