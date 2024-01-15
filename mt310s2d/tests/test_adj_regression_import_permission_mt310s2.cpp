#include "test_adj_regression_import_permission_mt310s2.h"
#include "factorycontrolleratmel.h"
#include "mockfactorycontroller.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include "mocki2ceepromiofactory.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_adj_regression_import_permission_mt310s2);

void test_adj_regression_import_permission_mt310s2::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_adj_regression_import_permission_mt310s2::scpiImportPermissionQueryFail()
{
    Zera::XMLConfig::cReader dummyReader;
    cI2CSettings i2cSettings(&dummyReader);
    setupServers(std::make_shared<FactoryControllerAtmel>(&i2cSettings));

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_adj_regression_import_permission_mt310s2::scpiImportNoPermission()
{
    setupServers(std::make_shared<MockFactoryController>(false));

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::erraut]);
}

void test_adj_regression_import_permission_mt310s2::scpiImportInvalidXml()
{
    setupServers(std::make_shared<MockFactoryController>(true));

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errxml]);
}

void test_adj_regression_import_permission_mt310s2::scpiImportFailFlashWrite()
{
    setupServers(std::make_shared<MockFactoryController>(true));

    QString xmlFileName = ":/import_minimal_pass.xml";
    QString xml = XmlHelperForTest::loadXml(xmlFileName);
    QVERIFY(!xml.isEmpty());

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", xml);
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_adj_regression_import_permission_mt310s2::scpiImportPassFlashWrite()
{
    MockI2cEEpromIoFactory::enableMock();
    setupServers(std::make_shared<MockFactoryController>(true));

    QString xmlFileName = ":/import_minimal_pass.xml";
    QString xml = XmlHelperForTest::loadXml(xmlFileName);
    QVERIFY(!xml.isEmpty());

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", xml);
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_adj_regression_import_permission_mt310s2::setupServers(FactoryControllerAbstractPtr ctrlFactory)
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceMt310s2>(ctrlFactory);
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}
