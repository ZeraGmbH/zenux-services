#include "test_regression_adj_status_com5003.h"
#include "adjflags.h"
#include "testadjstatussetter.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include "testfactoryi2cctrl.h"
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <mockeepromdevice.h>
#include <mocktcpnetworkfactory.h>
#include <QTest>

QTEST_MAIN(test_regression_adj_status_com5003);

void test_regression_adj_status_com5003::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
    qputenv("QT_FATAL_CRITICALS", "1");
    m_tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(m_tcpNetworkFactory);
}

void test_regression_adj_status_com5003::init()
{
    MockEepromDevice::cleanAll();
}

void test_regression_adj_status_com5003::cleanup()
{
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_status_com5003::statusDefaultMachine()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString ret = ScpiSingleTransactionBlocked::query("STATUS:PCB:ADJUSTMENT?");
    QCOMPARE(ret, QString::number(Adjustment::notAdjusted | Adjustment::wrongSNR));
}

void test_regression_adj_status_com5003::statusAllAdjusted()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QVERIFY(TestAdjStatusSetter::setAdjStatusAllChannelRanges(true));

    QString ret = ScpiSingleTransactionBlocked::query("STATUS:PCB:ADJUSTMENT?");
    QCOMPARE(ret, QString::number(Adjustment::adjusted));

    qInfo("Cleanup and read adjusted state...");
    cleanup();
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    ret = ScpiSingleTransactionBlocked::query("STATUS:PCB:ADJUSTMENT?");
    QCOMPARE(ret, QString::number(Adjustment::adjusted));
}

void test_regression_adj_status_com5003::setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    PermissionFunctions::setPermissionCtrlFactory(ctrlFactory);

    m_testServer = std::make_unique<TestServerForSenseInterfaceCom5003>(ctrlFactory, m_tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, m_tcpNetworkFactory);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
