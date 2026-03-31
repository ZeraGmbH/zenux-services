#include "test_generator_meas_range_notification.h"
#include "demofactoryi2cctrl.h"
#include "proxy.h"
#include "simulsystemstatus.h"
#include <mocktcpnetworkfactory.h>
#include <mockserverparamgenerator.h>
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_generator_meas_range_notification);

void test_generator_meas_range_notification::init()
{
    SimulSystemStatus *simulStatus = SimulSystemStatus::getInstance();
    simulStatus->setRange(1, 0);
    setupServers();
}

void test_generator_meas_range_notification::cleanup()
{
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_mt310s2d = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
}

static constexpr int notifierId = 42;
static constexpr int notifierIdOther = 37;

void test_generator_meas_range_notification::receiveSenseRangeChangeOnSourceRangeByAmplitudeChangeNotification()
{
    m_pcbIFace->registerNotifier("SENSE:M0:RANGE?", notifierId, true);
    m_pcbIFace->registerNotifier("SENSE:M1:RANGE?", notifierIdOther, true);
    m_pcbIFace->registerNotifier("SENSE:M2:RANGE?", notifierIdOther, true);
    m_pcbIFace->registerNotifier("SENSE:M3:RANGE?", notifierIdOther, true);
    m_pcbIFace->registerNotifier("SENSE:M4:RANGE?", notifierIdOther, true);
    m_pcbIFace->registerNotifier("SENSE:M5:RANGE?", notifierIdOther, true);
    m_pcbIFace->registerNotifier("SENSE:M6:RANGE?", notifierIdOther, true);
    m_pcbIFace->registerNotifier("SENSE:M7:RANGE?", notifierIdOther, true);
    TimeMachineObject::feedEventLoop();
    QSignalSpy spy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    SimulSystemStatus *simulStatus = SimulSystemStatus::getInstance();
    simulStatus->setRange(1, 2); // first channel is 1 / range 0 is default => to get a notification a change is needed
    m_pcbIFace->scpiCommand("GENERATOR:m0:AMPRANGE 2.5;");

    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 2);
    QString notificationStr = spy[0][2].toString();
    QCOMPARE(notificationStr, "Notify:42:125V"); // Mt581!
}

void test_generator_meas_range_notification::receiveSenseRangeChangeOnSourceRangeChangeNotification()
{
    m_pcbIFace->registerNotifier("SENSE:M0:RANGE?", notifierId, true);
    TimeMachineObject::feedEventLoop();
    QSignalSpy spy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    SimulSystemStatus *simulStatus = SimulSystemStatus::getInstance();
    simulStatus->setRange(1, 2); // first channel is 1 / range 0 is default => to get a notification a change is needed
    m_pcbIFace->scpiCommand("GENERATOR:m0:RANGE 3;");

    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 2);
    QString notificationStr = spy[0][2].toString();
    QCOMPARE(notificationStr, "Notify:42:125V"); // Mt581!
}

void test_generator_meas_range_notification::setupServers()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);

    ServerParams params = MockServerParamGenerator::createParams("mt581s2d");
    m_mt310s2d = std::make_unique<MockMt310s2d>(std::make_shared<DemoFactoryI2cCtrl>(std::make_unique<SettingsContainer>(params)),
                                                tcpNetworkFactory,
                                                "mt581s2");
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
