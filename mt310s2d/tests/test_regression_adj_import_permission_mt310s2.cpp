#include "test_regression_adj_import_permission_mt310s2.h"
#include "factoryi2cctrl.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include "mocki2ceepromiofactory.h"
#include <timemachineobject.h>
#include <mocktcpnetworkfactory.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_import_permission_mt310s2);

void test_regression_adj_import_permission_mt310s2::cleanup()
{
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_permission_mt310s2::scpiImportPermissionQueryFail()
{
    Zera::XMLConfig::cReader dummyReader;
    I2cSettings i2cSettings(&dummyReader);
    setupServers(std::make_shared<FactoryI2cCtrl>(&i2cSettings));

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_regression_adj_import_permission_mt310s2::scpiImportNoPermission()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(false));

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::erraut]);
}

void test_regression_adj_import_permission_mt310s2::scpiImportInvalidXml()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errxml]);
}

void test_regression_adj_import_permission_mt310s2::scpiImportFailFlashWrite()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString xmlFileName = ":/import_minimal_pass.xml";
    QString xml = XmlHelperForTest::loadXml(xmlFileName);
    QVERIFY(!xml.isEmpty());

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", xml);
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_regression_adj_import_permission_mt310s2::scpiImportPassFlashWrite()
{
    MockI2cEEpromIoFactory::enableMock();
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString xmlFileName = ":/import_minimal_pass.xml";
    QString xml = XmlHelperForTest::loadXml(xmlFileName);
    QVERIFY(!xml.isEmpty());

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", xml);
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_regression_adj_import_permission_mt310s2::setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resmanServer = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(ctrlFactory, tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
