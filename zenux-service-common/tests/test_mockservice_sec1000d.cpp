#include "test_mockservice_sec1000d.h"
#include "reply.h"
#include <timemachineobject.h>
#include <tcpnetworkfactory.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_mockservice_sec1000d)

void test_mockservice_sec1000d::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_mockservice_sec1000d::initTestCase_data()
{
    QTest::addColumn<int>("ecUnitCount");
    QTest::newRow("COM5003") << cSEC1000dServer::Com5003EcUnitCount;
    QTest::newRow("MTxxxs2") << cSEC1000dServer::Mtxxxs2EcUnitCount;
}

void test_mockservice_sec1000d::init()
{
    m_tcpNetworkFactory = VeinTcp::TcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(m_tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();
    QFETCH_GLOBAL(int, ecUnitCount);
    m_sec1000d = std::make_unique<MockSec1000d>(m_tcpNetworkFactory, ecUnitCount);
    TimeMachineObject::feedEventLoop();
    m_proxy = std::make_unique<ProxyForTest>();
}

void test_mockservice_sec1000d::cleanup()
{
    m_sec1000d = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
    m_proxy = nullptr;
}

Zera::cSECInterfacePtr test_mockservice_sec1000d::getSecInterface()
{
    Zera::cSECInterfacePtr secInterface = std::make_unique<Zera::cSECInterface>();
    Zera::ProxyClientPtr secClient = m_proxy->getConnectionSmart("127.0.0.1", 6305, m_tcpNetworkFactory);
    secInterface->setClientSmart(secClient);
    m_proxy->startConnectionSmart(secClient);
    TimeMachineObject::feedEventLoop();
    return secInterface;
}

void test_mockservice_sec1000d::getChannelCatSec1000d()
{
    Zera::cSECInterfacePtr secInterface = getSecInterface();
    QSignalSpy responseSpy(secInterface.get(), &AbstractServerInterface::serverAnswer);
    int msgNr = secInterface->getChannelCatalog();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QFETCH_GLOBAL(int, ecUnitCount);
    QString ecChannels;
    for (int i=0; i<ecUnitCount; ++i)
        ecChannels += QString("ec%1;").arg(i);
    QCOMPARE(responseSpy[0][2], QVariant(ecChannels));
}

void test_mockservice_sec1000d::freeSecResourcesWhenClientDisconnects()
{
    Zera::cSECInterfacePtr secInterface1 = getSecInterface();
    QSignalSpy responseSpy1(secInterface1.get(), &AbstractServerInterface::serverAnswer);
    secInterface1->setECalcUnit(4); //max 4 sec channels can be set at a time
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy1.count(), 1);
    QCOMPARE(responseSpy1[0][1], QVariant(ack));
    QCOMPARE(responseSpy1[0][2], QVariant("ec0;ec1;ec2;ec3;"));

    Zera::cSECInterfacePtr secInterface2 = getSecInterface();
    QSignalSpy responseSpy2(secInterface2.get(), &AbstractServerInterface::serverAnswer);
    secInterface2->setECalcUnit(4);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy2.count(), 1);
    QCOMPARE(responseSpy2[0][1], QVariant(ack));
    QCOMPARE(responseSpy2[0][2], QVariant("ec4;ec5;ec6;ec7;"));

    secInterface1 = nullptr;
    secInterface2 = nullptr;
    m_proxy = nullptr;

    m_proxy = std::make_unique<ProxyForTest>();

    Zera::cSECInterfacePtr secInterface3 = getSecInterface();
    QSignalSpy responseSpy3(secInterface3.get(), &AbstractServerInterface::serverAnswer);
    secInterface3->setECalcUnit(4);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy3.count(), 1);
    QCOMPARE(responseSpy3[0][1], QVariant(ack));
    QCOMPARE(responseSpy3[0][2], QVariant("ec0;ec1;ec2;ec3;"));
}
