#ifndef ZDSPCLIENTCONTAINER_H
#define ZDSPCLIENTCONTAINER_H

#include "zdspclient.h"

class ZDspClientContainer
{
public:
    virtual ~ZDspClientContainer();
    void addClient(VeinTcp::TcpPeer* netClient,
                   const QByteArray &proxyConnectionId,
                   AbstractFactoryDeviceNodeDspPtr deviceNodeFactory);

    const QList<ZdspClient*> &getClientList() const;
    ZdspClient *getFirstAdded() const; // is first mandatory / can we get rid of this?

    ZdspClient *findClient(quint16 dspInterruptId) const;
    ZdspClient *findClient(const QByteArray &proxyConnectionId) const;

    void delClients(const VeinTcp::TcpPeer *veinPeer);
    void delClient(const QByteArray &proxyConnectionId);
    void delAllClients();

private:
    QList<ZdspClient*> m_clientsChonological;
    QHash<QByteArray, ZdspClient*> m_clientsByProxyConnectionId;
    QHash<quint16, ZdspClient*> m_clientsByDspInterruptId;
    quint16 m_currentDspInterruptId = 0;
    void calcDspInterruptId();
};

#endif // ZDSPCLIENTCONTAINER_H
