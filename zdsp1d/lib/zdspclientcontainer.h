#ifndef ZDSPCLIENTCONTAINER_H
#define ZDSPCLIENTCONTAINER_H

#include "dspvardevicenodeinout.h"
#include "zdspclient.h"
#include <logstatisticsasyncint.h>

class ZDspClientContainer
{
public:
    explicit ZDspClientContainer(AbstractFactoryZdspSupportPtr zdspSupportFactory);
    virtual ~ZDspClientContainer();
    void addClient(VeinTcp::TcpPeer* netClient,
                   const QByteArray &proxyConnectionId);
    bool makeSuperClient(const ZdspClient *dspClient);

    const QList<ZdspClient*> &getClientList() const;
    ZdspClient *getFirstAdded() const; // is first mandatory / can we get rid of this?

    ZdspClient *findClient(quint16 dspInterruptId) const;
    ZdspClient *findClient(const QByteArray &proxyConnectionId) const;

    void delClients(const VeinTcp::TcpPeer *veinPeer);
    void delClient(const QByteArray &proxyConnectionId);
    void delAllClients();

    void handleDspInterrupt(DspVarDeviceNodeInOut &dspInOut) const;

private:
    void calcDspInterruptId();
    void resetInterruptIdOnNoClients();

    AbstractFactoryZdspSupportPtr m_zdspSupportFactory;
    QList<ZdspClient*> m_clientsChonological;
    QHash<QByteArray, ZdspClient*> m_clientsByProxyConnectionId;
    QHash<quint16, ZdspClient*> m_clientsByDspInterruptId;
    const ZdspClient *m_dspSuperClient = nullptr;
    quint16 m_currentDspInterruptId = 0;
    LogStatisticsAsyncInt m_dspInterruptLogStatistics;
};

#endif // ZDSPCLIENTCONTAINER_H
