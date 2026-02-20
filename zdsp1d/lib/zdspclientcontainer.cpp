#include "zdspclientcontainer.h"

ZDspClientContainer::ZDspClientContainer(AbstractFactoryZdspSupportPtr zdspSupportFactory) :
    m_zdspSupportFactory(zdspSupportFactory)
{
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
}

void ZDspClientContainer::delAllClients()
{
    for (ZdspClient* client : qAsConst(m_clientsChonological))
        delete client;
    m_clientsChonological.clear();
    m_clientsByProxyConnectionId.clear();
    m_clientsByDspInterruptId.clear();
}

void ZDspClientContainer::calcDspInterruptId()
{
    m_currentDspInterruptId++;
    if (m_currentDspInterruptId == 0)
        m_currentDspInterruptId++;
}
