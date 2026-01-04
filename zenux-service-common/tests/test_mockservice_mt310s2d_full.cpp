#include "test_mockservice_mt310s2d_full.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "reply.h"
#include <timemachineobject.h>
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include <mockeepromdevice.h>
#include <mocktcpnetworkfactory.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_mockservice_mt310s2d_full)


void test_mockservice_mt310s2d_full::initTestCase()
{
    //qputenv("QT_FATAL_CRITICALS", "1");
}

void test_mockservice_mt310s2d_full::initTestCase_data()
{
    QTest::addColumn<QString>("serviceNameForAlternateDevice");
    QTest::newRow("MT310s2") << QString("mt310s2d");
    QTest::newRow("MT581s2") << QString("mt581s2d");
}

void test_mockservice_mt310s2d_full::init()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    QFETCH_GLOBAL(QString, serviceNameForAlternateDevice);
    m_mt310s2d = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), tcpNetworkFactory, serviceNameForAlternateDevice);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

void test_mockservice_mt310s2d_full::cleanup()
{
    MockEepromDevice::cleanAll();
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_mt310s2d = nullptr;
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_mockservice_mt310s2d_full::connectServer()
{
    Zera::cPCBInterface pcbIFace;
    pcbIFace.setClientSmart(m_proxyClient);

    QSignalSpy connectSpy(m_proxyClient.get(), &Zera::ProxyClient::connected);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(connectSpy.count(), 1);
}

void test_mockservice_mt310s2d_full::smokeTestSystemInterface()
{
    // That caused biseceting sessions: system interface uses uninitialized sense interface
    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:WRITE", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_mockservice_mt310s2d_full::smokeTestStatusInterface()
{
    // That caused biseceting sessions: system interface uses uninitialized sense interface
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    int msgNr = m_pcbIFace->getAdjustmentStatus();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    // we are smoke test: Ignore adjustment status
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
}

void test_mockservice_mt310s2d_full::smokeTestSenseInterface()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    int msgNr = m_pcbIFace->getChannelList();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QStringList() << "m0" << "m1" << "m2" << "m6" << "m3" << "m4" << "m5" << "m7");
}


void test_mockservice_mt310s2d_full::smokeTestFoutGroupChannelResourceAndInterface()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    int msgNr = m_pcbIFace->getSourceCat();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("fo0;fo1;fo2;fo3"));
}

