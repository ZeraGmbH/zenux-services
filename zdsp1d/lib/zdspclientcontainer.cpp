#include "zdspclientcontainer.h"

ZDspClientContainer::ZDspClientContainer(AbstractFactoryZdspSupportPtr zdspSupportFactory) :
    m_zdspSupportFactory(zdspSupportFactory),
    m_dspDspInterruptStatisticsGenerator(10000)
{
    QObject::connect(&m_dspDspInterruptStatisticsGenerator, &LogStatisticsAsyncInt::sigNewStatistics, [&](int min, int max, float avg,
                                                                                                      int linuxInterruptCount) {
        m_dspIoStatisticsLastPeriod.minDspInterruptsPerLinuxInterrupt = min;
        m_dspIoStatisticsLastPeriod.maxDspInterruptsPerLinuxInterrupt = max;
        m_dspIoStatisticsLastPeriod.avgDspInterruptsPerLinuxInterrupt = avg;
        m_dspIoStatisticsLastPeriod.linuxInterruptCount = linuxInterruptCount;

        m_dspIoStatisticsLastPeriod.dspReadTransactions = AbstractDspDeviceNode::getReadTransactions();
        m_dspIoStatisticsLastPeriod.dspWriteTransactions = AbstractDspDeviceNode::getWriteTransactions();
        AbstractDspDeviceNode::resetAllTransactions();
    });
}

ZDspClientContainer::~ZDspClientContainer()
{
    delAllClients();
}

void ZDspClientContainer::addClient(VeinTcp::TcpPeer *netClient, const QByteArray &proxyConnectionId)
{
    if (m_clientsByProxyConnectionId.contains(proxyConnectionId))
        return;
    calcDspInterruptId();
    if (m_clientsByDspInterruptId.contains(m_currentDspInterruptId)) {
        qCritical("DSP interrupt id %i already used!", m_currentDspInterruptId);
        return;
    }
    ZdspClient* client = new ZdspClient(m_currentDspInterruptId,
                                        netClient,
                                        proxyConnectionId,
                                        m_zdspSupportFactory);
    m_clientsChonological.append(client);
    m_clientsByDspInterruptId[m_currentDspInterruptId] = client;
    m_clientsByProxyConnectionId[proxyConnectionId] = client;
}

bool ZDspClientContainer::makeSuperClient(const ZdspClient *dspClient)
{
    if (m_dspSuperClient)
        return false;
    m_dspSuperClient = dspClient;
    QObject::connect(m_dspSuperClient, &QObject::destroyed, dspClient, [&]() {
        m_dspSuperClient = nullptr;
    });
    return true;
}

const QList<ZdspClient *> &ZDspClientContainer::getClientList() const
{
    return m_clientsChonological;
}

ZdspClient *ZDspClientContainer::getFirstAdded() const
{
    if (!m_clientsChonological.isEmpty())
        return m_clientsChonological.first();
    return nullptr;
}

ZdspClient *ZDspClientContainer::findClient(quint16 dspInterruptId) const
{
    auto iter = m_clientsByDspInterruptId.constFind(dspInterruptId);
    if (iter != m_clientsByDspInterruptId.constEnd())
        return iter.value();
    return nullptr;
}

ZdspClient *ZDspClientContainer::findClient(const QByteArray &proxyConnectionId) const
{
    auto iter = m_clientsByProxyConnectionId.constFind(proxyConnectionId);
    if (iter != m_clientsByProxyConnectionId.constEnd())
        return iter.value();
    return nullptr;
}

void ZDspClientContainer::delClients(const VeinTcp::TcpPeer *veinPeer)
{
    QList<const ZdspClient*> clientsToDelete;
    for (ZdspClient* client : qAsConst(m_clientsChonological))
        if (client->getVeinPeer() == veinPeer)
            clientsToDelete.append(client);

    for (const ZdspClient* clientToDelete : clientsToDelete)
        delClient(clientToDelete->getProtobufClientId());
    resetInterruptIdOnNoClients();
}

void ZDspClientContainer::delClient(const QByteArray &proxyConnectionId)
{
    ZdspClient *client = findClient(proxyConnectionId);
    if (client == nullptr)
        return;
    m_clientsChonological.removeAll(client);
    m_clientsByProxyConnectionId.remove(client->getProtobufClientId());
    m_clientsByDspInterruptId.remove(client->getDspInterruptId());
    delete client;
    resetInterruptIdOnNoClients();
}

void ZDspClientContainer::delAllClients()
{
    for (ZdspClient* client : qAsConst(m_clientsChonological))
        delete client;
    m_clientsChonological.clear();
    m_clientsByProxyConnectionId.clear();
    m_clientsByDspInterruptId.clear();
    resetInterruptIdOnNoClients();
}

void ZDspClientContainer::handleDspInterrupt(DspVarDeviceNodeInOut &dspInOut)
{
    const QList<ZdspClient*> clientList = getClientList();
    if (!clientList.isEmpty()) { // wenn vorhanden nutzen wir immer den 1. client zum lesen
        ZdspClient *client = clientList.first();
        QByteArray ba;
        if (dspInOut.readOneDspVar(QString("CTRLCMDPAR,%1").arg(DSP_INTERRUPT_PARAM_BUFFER_LEN),
                                     &ba, &client->m_dspVarResolver)) {
            const ulong* pardsp = reinterpret_cast<ulong*>(ba.data());
            int interruptCount = pardsp[0];
            m_dspDspInterruptStatisticsGenerator.addValue(interruptCount);
            if (interruptCount > DSP_MAX_PENDING_INTERRUPT_COUNT)
                qWarning("Number of interrupts in a package: %i exceeds upper limit!", interruptCount);
            else {
                if (m_dspSuperClient) { // notify super client first
                    bool superClientFound = false;
                    // search super client index - it is expected last => start search at end
                    for (int i = interruptCount; i >= 1; i--) {
                        int process = pardsp[i] >> 16;
                        const ZdspClient *clientToNotify = findClient(process);
                        if (clientToNotify && clientToNotify == m_dspSuperClient) {
                            clientToNotify->sendInterruptNotification(pardsp[interruptCount]);
                            superClientFound = true;
                            break;
                        }
                    }
                    if (!superClientFound)
                        qWarning("Super client not found!");
                }
                for (int i = 1; i < (interruptCount+1); i++) {
                    int process = pardsp[i] >> 16;
                    const ZdspClient *clientToNotify = findClient(process);
                    if (clientToNotify && clientToNotify != m_dspSuperClient) // don't double notify super client
                        clientToNotify->sendInterruptNotification(pardsp[i]);
                }
            }
        }
        dspInOut.writeDspVars(QString("CTRLACK,%1;").arg(CmdDone), &client->m_dspVarResolver); // jetzt in jedem fall acknowledge
    }
    else {
        DspVarResolver dspSystemVarResolver;
        dspInOut.writeDspVars(QString("CTRLACK,%1;").arg(CmdDone), &dspSystemVarResolver); // und rücksetzen
    }

}

void ZDspClientContainer::calcDspInterruptId()
{
    m_currentDspInterruptId++;
    if (m_currentDspInterruptId == 0)
        m_currentDspInterruptId++;
}

void ZDspClientContainer::resetInterruptIdOnNoClients()
{
    if (m_clientsByDspInterruptId.isEmpty())
        m_currentDspInterruptId = 0;
}
