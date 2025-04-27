#include "test_zdspclientcontainer.h"
#include "zdspclientcontainer.h"
#include "testfactorydevicenodedsp.h"
#include <QTest>

QTEST_MAIN(test_zdspclientcontainer);

void test_zdspclientcontainer::initTestCase()
{
    m_devNodeFactory = std::make_shared<TestFactoryDeviceNodeDsp>();
}

void test_zdspclientcontainer::emptyReturnsEmptyList()
{
    ZDspClientContainer container;
    QCOMPARE(container.getClientList().count(), 0);
}

void test_zdspclientcontainer::emptyReturnsNullFirst()
{
    ZDspClientContainer container;
    QCOMPARE(container.getFirstAdded(), nullptr);
}

void test_zdspclientcontainer::emptyReturnsNullFindDspInterruptId()
{
    ZDspClientContainer container;
    QCOMPARE(container.findClient(1), nullptr);
}

void test_zdspclientcontainer::emptyReturnsNullFindProxyConnectionId()
{
    ZDspClientContainer container;
    QCOMPARE(container.findClient("proxyConnectionId0"), nullptr);
}

void test_zdspclientcontainer::getList()
{
    VeinTcp::TcpPeer peer(m_netFactory.create());
    ZDspClientContainer container;

    container.addClient(&peer, "proxyConnectionId0", m_devNodeFactory);
    container.addClient(&peer, "proxyConnectionId1", m_devNodeFactory);
    QCOMPARE(container.getClientList().count(), 2);

    container.delAllClients();
    QCOMPARE(container.getClientList().count(), 0);

    container.addClient(&peer, "proxyConnectionId3", m_devNodeFactory);
    container.addClient(&peer, "proxyConnectionId4", m_devNodeFactory);
    QCOMPARE(container.getClientList().count(), 2);
}

void test_zdspclientcontainer::getFirst()
{
    VeinTcp::TcpPeer peer(m_netFactory.create());
    ZDspClientContainer container;

    container.addClient(&peer, "proxyConnectionId0", m_devNodeFactory);
    container.addClient(&peer, "proxyConnectionId1", m_devNodeFactory);

    ZdspClient* client = container.getFirstAdded();
    QCOMPARE(client->getProtobufClientId(), "proxyConnectionId0");

    container.delAllClients();
    QCOMPARE(container.getFirstAdded(), nullptr);

    container.addClient(&peer, "proxyConnectionId3", m_devNodeFactory);
    container.addClient(&peer, "proxyConnectionId4", m_devNodeFactory);

    client = container.getFirstAdded();
    QCOMPARE(client->getProtobufClientId(), "proxyConnectionId3");
}

void test_zdspclientcontainer::checkDspInterruptIds()
{
    VeinTcp::TcpPeer peer(m_netFactory.create());
    ZDspClientContainer container;

    constexpr int maxIds = 0xFFFF;
    for (int i=0; i<maxIds; i++)
        container.addClient(&peer, QString("proxyConnectionId%1").arg(i).toLatin1(), m_devNodeFactory);

    QList<ZdspClient*> clients = container.getClientList();
    QCOMPARE(clients.count(), maxIds);

    QSet<int> ids;
    bool allNonZero = true;
    for (int i=0; i<maxIds; i++) {
        int id = clients[i]->getDspInterruptId();
        ids.insert(id);
        if (id==0)
            allNonZero = false;
    }
    QVERIFY(allNonZero);

    bool allIdsPossibleFound = true;
    for (int i=1; i<=maxIds; i++) {
        if(!ids.contains(i))
            allIdsPossibleFound = false;
    }
    QVERIFY(allIdsPossibleFound);

    container.delAllClients();
    for (int i=0; i<10; i++)
        container.addClient(&peer, QString("proxyConnectionId%1").arg(i).toLatin1(), m_devNodeFactory);
    clients = container.getClientList();
    QCOMPARE(clients.count(), 10);
}

void test_zdspclientcontainer::checkDspInterruptIdsExceedMax()
{
    VeinTcp::TcpPeer peer(m_netFactory.create());
    ZDspClientContainer container;

    constexpr int maxIds = 0xFFFF;
    for (int i=0; i<maxIds+3; i++)
        container.addClient(&peer, QString("proxyConnectionId%1").arg(i).toLatin1(), m_devNodeFactory);

    QList<ZdspClient*> clients = container.getClientList();
    QCOMPARE(clients.count(), maxIds);
}

void test_zdspclientcontainer::addRemoveDeletesClient()
{
    VeinTcp::TcpPeer peer(m_netFactory.create());
    ZDspClientContainer container;

    container.addClient(&peer, "proxyConnectionId0", m_devNodeFactory);
    container.addClient(&peer, "proxyConnectionId1", m_devNodeFactory);
    QCOMPARE(ZdspClient::getInstanceCount(), 2);

    container.delAllClients();
    QCOMPARE(ZdspClient::getInstanceCount(), 0);
}

void test_zdspclientcontainer::addSameClientIdIgnored()
{
    VeinTcp::TcpPeer peer(m_netFactory.create());
    ZDspClientContainer container;

    container.addClient(&peer, "proxyConnectionId", m_devNodeFactory);
    QCOMPARE(ZdspClient::getInstanceCount(), 1);
    ZdspClient *client1 = container.findClient("proxyConnectionId");
    QCOMPARE(client1->getProtobufClientId(), "proxyConnectionId");
    int interruptId1 = client1->getDspInterruptId();

    container.addClient(&peer, "proxyConnectionId", m_devNodeFactory);
    QCOMPARE(ZdspClient::getInstanceCount(), 1);
    ZdspClient *client2 = container.findClient("proxyConnectionId");
    QCOMPARE(client2->getProtobufClientId(), "proxyConnectionId");
    int interruptId2 = client2->getDspInterruptId();
    QVERIFY(interruptId1 == interruptId2);
}

void test_zdspclientcontainer::findByProxyConnectionId()
{
    VeinTcp::TcpPeer peer1(m_netFactory.create());
    VeinTcp::TcpPeer peer2(m_netFactory.create());
    ZDspClientContainer container;

    container.addClient(&peer1, "proxyConnectionId0", m_devNodeFactory);
    container.addClient(&peer2, "proxyConnectionId1", m_devNodeFactory);

    ZdspClient *client1 = container.findClient("proxyConnectionId0");
    QCOMPARE(client1->getProtobufClientId(), "proxyConnectionId0");
    QCOMPARE(client1->getVeinPeer(), &peer1);
    ZdspClient *client2 = container.findClient("proxyConnectionId1");
    QCOMPARE(client2->getProtobufClientId(), "proxyConnectionId1");
    QCOMPARE(client2->getVeinPeer(), &peer2);

    container.delAllClients();
    QCOMPARE(container.findClient("proxyConnectionId0"), nullptr);
    QCOMPARE(container.findClient("proxyConnectionId1"), nullptr);
}

void test_zdspclientcontainer::findByDspInterruptId()
{
    VeinTcp::TcpPeer peer(m_netFactory.create());
    ZDspClientContainer container;

    container.addClient(&peer, "proxyConnectionId0", m_devNodeFactory);
    container.addClient(&peer, "proxyConnectionId1", m_devNodeFactory);
    QList<ZdspClient*> clients = container.getClientList();

    int dspInterruptId1 = clients[0]->getDspInterruptId();
    QCOMPARE(container.findClient(dspInterruptId1), clients[0]);
    int dspInterruptId2 = clients[1]->getDspInterruptId();
    QCOMPARE(container.findClient(dspInterruptId2), clients[1]);

    container.delAllClients();
    QCOMPARE(container.findClient(dspInterruptId1), nullptr);
    QCOMPARE(container.findClient(dspInterruptId2), nullptr);
}

void test_zdspclientcontainer::deleteByProxyConnectionIdNotAdded()
{
    VeinTcp::TcpPeer peer(m_netFactory.create());
    ZDspClientContainer container;

    container.addClient(&peer, "proxyConnectionId0", m_devNodeFactory);
    container.addClient(&peer, "proxyConnectionId1", m_devNodeFactory);

    container.delClient("");
    QCOMPARE(container.getClientList().count(), 2);
}

void test_zdspclientcontainer::deleteByProxyConnectionId()
{
    VeinTcp::TcpPeer peer(m_netFactory.create());
    ZDspClientContainer container;

    container.addClient(&peer, "proxyConnectionId0", m_devNodeFactory);
    container.addClient(&peer, "proxyConnectionId1", m_devNodeFactory);
    QList<ZdspClient*> clientsBeforeRemove = container.getClientList();
    const int interruptId1 = clientsBeforeRemove[0]->getDspInterruptId();
    const int interruptId2 = clientsBeforeRemove[1]->getDspInterruptId();

    container.delClient("proxyConnectionId0");
    QList<ZdspClient*> clientsAfterRemove1 = container.getClientList();

    QCOMPARE(clientsAfterRemove1.count(), 1);
    QCOMPARE(clientsAfterRemove1[0]->getProtobufClientId(), "proxyConnectionId1");
    QCOMPARE(container.getFirstAdded(), clientsAfterRemove1[0]);

    QCOMPARE(container.findClient("proxyConnectionId0"), nullptr);
    QCOMPARE(container.findClient("proxyConnectionId1")->getProtobufClientId(), "proxyConnectionId1");

    QCOMPARE(container.findClient(interruptId1), nullptr);
    QCOMPARE(container.findClient(interruptId2)->getDspInterruptId(), interruptId2);

    QCOMPARE(ZdspClient::getInstanceCount(), 1);


    container.delClient("proxyConnectionId1");
    QList<ZdspClient*> clientsAfterRemove2 = container.getClientList();

    QCOMPARE(clientsAfterRemove2.count(), 0);
    QCOMPARE(container.getFirstAdded(), nullptr);

    QCOMPARE(container.findClient("proxyConnectionId0"), nullptr);
    QCOMPARE(container.findClient("proxyConnectionId1"), nullptr);

    QCOMPARE(container.findClient(interruptId1), nullptr);
    QCOMPARE(container.findClient(interruptId2), nullptr);

    QCOMPARE(ZdspClient::getInstanceCount(), 0);
}

void test_zdspclientcontainer::deleteByVeinPeerNotAdded()
{
    VeinTcp::TcpPeer peerAdded(m_netFactory.create());
    ZDspClientContainer container;

    container.addClient(&peerAdded, "proxyConnectionId0", m_devNodeFactory);
    container.addClient(&peerAdded, "proxyConnectionId1", m_devNodeFactory);

    VeinTcp::TcpPeer peerNotAdded(m_netFactory.create());
    container.delClients(&peerNotAdded);
    QCOMPARE(container.getClientList().count(), 2);
}

void test_zdspclientcontainer::deleteByVeinPeer()
{
    VeinTcp::TcpPeer peer0(m_netFactory.create());
    VeinTcp::TcpPeer peer1(m_netFactory.create());
    ZDspClientContainer container;

    container.addClient(&peer0, "proxyConnectionId0", m_devNodeFactory);
    container.addClient(&peer0, "proxyConnectionId1", m_devNodeFactory);
    container.addClient(&peer1, "proxyConnectionId2", m_devNodeFactory);
    container.addClient(&peer1, "proxyConnectionId3", m_devNodeFactory);
    QList<ZdspClient*> clientsBeforeRemove = container.getClientList();
    const int interruptId0 = clientsBeforeRemove[0]->getDspInterruptId();
    const int interruptId1 = clientsBeforeRemove[1]->getDspInterruptId();
    const int interruptId2 = clientsBeforeRemove[2]->getDspInterruptId();
    const int interruptId3 = clientsBeforeRemove[3]->getDspInterruptId();

    container.delClients(&peer0);
    QList<ZdspClient*> clientsAfterRemove1 = container.getClientList();

    QCOMPARE(clientsAfterRemove1.count(), 2);
    QCOMPARE(clientsAfterRemove1[0]->getProtobufClientId(), "proxyConnectionId2");
    QCOMPARE(clientsAfterRemove1[1]->getProtobufClientId(), "proxyConnectionId3");
    QCOMPARE(container.getFirstAdded(), clientsAfterRemove1[0]);

    QCOMPARE(container.findClient("proxyConnectionId0"), nullptr);
    QCOMPARE(container.findClient("proxyConnectionId1"), nullptr);
    QCOMPARE(container.findClient("proxyConnectionId2")->getProtobufClientId(), "proxyConnectionId2");
    QCOMPARE(container.findClient("proxyConnectionId3")->getProtobufClientId(), "proxyConnectionId3");

    QCOMPARE(container.findClient(interruptId0), nullptr);
    QCOMPARE(container.findClient(interruptId1), nullptr);
    QCOMPARE(container.findClient(interruptId2)->getDspInterruptId(), interruptId2);
    QCOMPARE(container.findClient(interruptId3)->getDspInterruptId(), interruptId3);

    QCOMPARE(ZdspClient::getInstanceCount(), 2);

    container.delClients(&peer1);
    QList<ZdspClient*> clientsAfterRemove2 = container.getClientList();

    QCOMPARE(clientsAfterRemove2.count(), 0);
    QCOMPARE(container.getFirstAdded(), nullptr);

    QCOMPARE(container.findClient("proxyConnectionId0"), nullptr);
    QCOMPARE(container.findClient("proxyConnectionId1"), nullptr);
    QCOMPARE(container.findClient("proxyConnectionId2"), nullptr);
    QCOMPARE(container.findClient("proxyConnectionId3"), nullptr);

    QCOMPARE(container.findClient(interruptId0), nullptr);
    QCOMPARE(container.findClient(interruptId1), nullptr);
    QCOMPARE(container.findClient(interruptId2), nullptr);
    QCOMPARE(container.findClient(interruptId3), nullptr);

    QCOMPARE(ZdspClient::getInstanceCount(), 0);
}

void test_zdspclientcontainer::cleanup()
{
    QCOMPARE(ZdspClient::getInstanceCount(), 0);
}
