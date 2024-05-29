#include "test_regression_adj_status_com5003.h"
#include "adjflags.h"
#include "testadjstatussetter.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include "mocki2ceepromiofactory.h"
#include "testfactoryi2cctrl.h"
#include "zscpi_response_definitions.h"
#include <timemachineobject.h>
#include <mockeeprom24lc.h>
#include <QTest>

QTEST_MAIN(test_regression_adj_status_com5003);

void test_regression_adj_status_com5003::initTestCase()
{
    MockI2cEEpromIoFactory::enableMock();
}

void test_regression_adj_status_com5003::init()
{
    MockEEprom24LC::mockCleanAll();
}

void test_regression_adj_status_com5003::cleanup()
{
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
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
    // TEMP: THIS and all the others of this kind MUST GO!!!
    QCOMPARE(ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", ""), ZSCPI::scpiAnswer[ZSCPI::ack]);
    ret = ScpiSingleTransactionBlocked::query("STATUS:PCB:ADJUSTMENT?");
    QCOMPARE(ret, QString::number(Adjustment::adjusted));
}

void test_regression_adj_status_com5003::setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    PermissionFunctions::setPermissionCtrlFactory(ctrlFactory);

    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_testServer = std::make_unique<TestServerForSenseInterfaceCom5003>(ctrlFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
