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

    void handleDspInterrupt(DspVarDeviceNodeInOut &dspInOut);

private:
    void calcDspInterruptId();
    void resetInterruptIdOnNoClients();

    AbstractFactoryZdspSupportPtr m_zdspSupportFactory;
    QList<ZdspClient*> m_clientsChonological;
    QHash<QByteArray, ZdspClient*> m_clientsByProxyConnectionId;
    QHash<quint16, ZdspClient*> m_clientsByDspInterruptId;
    const ZdspClient *m_dspSuperClient = nullptr;
    quint16 m_currentDspInterruptId = 0;

    // We logged interrupt statistics and did interesting analysis long ago
    // Currently logged data is not analysed anmore. To enable future analysis
    // we just keep data and don't use it.
    LogStatisticsAsyncInt m_dspInterruptStatisticGenerator;
    struct InterruptStatistic {
        int minDspInterruptsPerLinuxInterrupt = 0;
        int maxDspInterruptsPerLinuxInterrupt = 0;
        float avgDspInterruptsPerLinuxInterrupt = 0.0;
        int linuxInterruptCount = 0;
    };
    InterruptStatistic m_interruptStatisticLastPeriod;
};

#endif // ZDSPCLIENTCONTAINER_H
