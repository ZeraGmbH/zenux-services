#include "test_mockservice_com5003d_full.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "reply.h"
#include <timemachineobject.h>
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include <mockeeprom24lc.h>
#include <tcpworkerfactory.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_mockservice_com5003d_full)

void test_mockservice_com5003d_full::initTestCase()
{
    //qputenv("QT_FATAL_CRITICALS", "1");
}

void test_mockservice_com5003d_full::init()
{
    VeinTcp::AbstractTcpWorkerFactoryPtr tcpNetworkFactory = VeinTcp::TcpWorkerFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_server = std::make_unique<MockCom5003d>(std::make_shared<TestFactoryI2cCtrl>(true), tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

void test_mockservice_com5003d_full::cleanup()
{
    MockEEprom24LC::mockCleanAll();
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_server = nullptr;
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_mockservice_com5003d_full::connectServer()
{
    Zera::cPCBInterface pcbIFace;
    pcbIFace.setClientSmart(m_proxyClient);

    QSignalSpy connectSpy(m_proxyClient.get(), &Zera::ProxyClient::connected);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(connectSpy.count(), 1);
}

void test_mockservice_com5003d_full::smokeTestSystemInterface()
{
    // That caused biseceting sessions: system interface uses uninitialized sense interface
    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:WRITE", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_mockservice_com5003d_full::smokeTestStatusInterface()
{
    // That caused biseceting sessions: system interface uses uninitialized sense interface
    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    int msgNr = m_pcbIFace->getAdjustmentStatus();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    // we are smoke test: Ignore adjustment status
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
}

void test_mockservice_com5003d_full::smokeTestSenseInterface()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    int msgNr = m_pcbIFace->getChannelList();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariantList() << "m0" << "m1" << "m2" << "m3" << "m4" << "m5");
}

void test_mockservice_com5003d_full::smokeTestFoutGroupChannelResourceAndInterface()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    int msgNr = m_pcbIFace->getSourceCat();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("fo0;fo1;fo2;fo3"));
}
