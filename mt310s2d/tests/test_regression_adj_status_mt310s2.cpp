#include "test_regression_adj_status_mt310s2.h"
#include "adjustmentflags.h"
#include "testadjstatussetter.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include "mocki2ceepromiofactory.h"
#include "clampfactorytest.h"
#include "testfactoryi2cctrl.h"
#include "zscpi_response_definitions.h"
#include <timemachineobject.h>
#include <mockeeprom24lc.h>
#include <QTest>

QTEST_MAIN(test_regression_adj_status_mt310s2);

void test_regression_adj_status_mt310s2::initTestCase()
{
    ClampFactoryTest::enableTest();
    MockI2cEEpromIoFactory::enableMock();
}

void test_regression_adj_status_mt310s2::cleanup()
{
    MockEEprom24LC::cleanAll();
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_status_mt310s2::statusDefaultMachine()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString ret = ScpiSingleTransactionBlocked::query("STATUS:PCB:ADJUSTMENT?");
    QCOMPARE(ret, QString::number(Adjustment::notAdjusted | Adjustment::wrongSNR));
}

void test_regression_adj_status_mt310s2::statusAllAdjusted()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QVERIFY(TestAdjStatusSetter::setAdjStatusAllChannelRanges(true));

    QString ret = ScpiSingleTransactionBlocked::query("STATUS:PCB:ADJUSTMENT?");
    QCOMPARE(ret, QString::number(Adjustment::adjusted));
}

void test_regression_adj_status_mt310s2::statusClampsI1DefaultMachine()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString ret = ScpiSingleTransactionBlocked::query("STATUS:CLAMP:m3:ADJUSTMENT?");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_adj_status_mt310s2::statusClampsI1DefaultCL120A()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    m_testServer->addClamp(cClamp::CL120A, "IL1");

    QString ret = ScpiSingleTransactionBlocked::query("STATUS:CLAMP:m3:ADJUSTMENT?");
    QCOMPARE(ret, QString::number(Adjustment::notAdjusted));
}

void test_regression_adj_status_mt310s2::setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    PermissionFunctions::setPermissionCtrlFactory(ctrlFactory);

    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(ctrlFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
