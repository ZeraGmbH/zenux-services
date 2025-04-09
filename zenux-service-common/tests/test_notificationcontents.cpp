#include "test_notificationcontents.h"
#include "testfactoryi2cctrl.h"
#include "networkconnectioninfo.h"
#include "clampfactorytest.h"
#include "mocki2ceepromiofactory.h"
#include "proxy.h"
#include <mocktcpnetworkfactory.h>
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_notificationcontents)

static const NetworkConnectionInfo netInfo("127.0.0.1", 6307);

using namespace Zera;

void test_notificationcontents::initTestCase()
{
    ClampFactoryTest::enableTest();
    MockI2cEEpromIoFactory::enableMock();
    m_tcpFactory = VeinTcp::MockTcpNetworkFactory::create();
    TimerFactoryQtForTest::enableTest();
}

void test_notificationcontents::init()
{
    setupServers();
    setupClient();
}

void test_notificationcontents::cleanup()
{
    m_pcbInterface = nullptr;
    m_pcbClient = nullptr;
    TimeMachineObject::feedEventLoop();
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

static constexpr int notifierId = 42;

void test_notificationcontents::rangeChangeWithoutValue()
{
    m_pcbInterface->registerNotifier("SENSE:m0:RANGE?", notifierId, false);
    TimeMachineObject::feedEventLoop();
    QSignalSpy spy(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer);

    m_pcbInterface->setRange("m0", "8V");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 2 /* notification first + change range response */);
    QString notificationStr = spy[0][2].toString();
    QCOMPARE(notificationStr, "Notify:42");
}

void test_notificationcontents::rangeChangeWithValue()
{
    m_pcbInterface->registerNotifier("SENSE:m0:RANGE?", notifierId, true);
    TimeMachineObject::feedEventLoop();
    QSignalSpy spy(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer);

    m_pcbInterface->setRange("m0", "8V");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 2);
    QString notificationStr = spy[0][2].toString();
    QCOMPARE(notificationStr, "Notify:42:8V");
}

void test_notificationcontents::rangesChangeWithoutValue()
{
    m_pcbInterface->registerNotifier("SENSE:m0:RANGE:CATALOG?", notifierId, false);
    TimeMachineObject::feedEventLoop();
    QSignalSpy spy(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer);

    m_pcbInterface->setMMode("ADJ");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 2);
    QString notificationStr = spy[0][2].toString();
    // We should make this fail: Why is this send out - there is no change!!!
    QCOMPARE(notificationStr, "Notify:42");
}

void test_notificationcontents::rangesChangeWithValue()
{
    m_pcbInterface->registerNotifier("SENSE:m0:RANGE:CATALOG?", notifierId, true);
    TimeMachineObject::feedEventLoop();
    QSignalSpy spy(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer);

    m_pcbInterface->setMMode("ADJ");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 2);
    QString notificationStr = spy[0][2].toString();
    // We should make this fail: Why is this send out - there is no change!!!
    QCOMPARE(notificationStr, "Notify:42:250V;8V;100mV");
}

void test_notificationcontents::setupServers()
{
    TimeMachineForTest::reset();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(
        std::make_shared<TestFactoryI2cCtrl>(true), m_tcpFactory);
    TimeMachineObject::feedEventLoop();
}

void test_notificationcontents::setupClient()
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(netInfo, m_tcpFactory);
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_pcbInterface->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}
