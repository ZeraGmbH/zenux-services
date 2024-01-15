#include "test_adj_regression_import_permission_com5003.h"
#include "atmelctrlfactory.h"
#include "mockatmelctrlfactory.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include "mocki2ceepromiofactory.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_adj_regression_import_permission_com5003);

void test_adj_regression_import_permission_com5003::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_adj_regression_import_permission_com5003::scpiImportPermissionQueryFail()
{
    Zera::XMLConfig::cReader dummyReader;
    cI2CSettings i2cSettings(&dummyReader);
    setupServers(std::make_shared<AtmelCtrlFactory>(&i2cSettings));

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_adj_regression_import_permission_com5003::scpiImportNoPermission()
{
    setupServers(std::make_shared<MockAtmelCtrlFactory>(false));

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::erraut]);
}

void test_adj_regression_import_permission_com5003::scpiImportInvalidXml()
{
    setupServers(std::make_shared<MockAtmelCtrlFactory>(true));

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errxml]);
}

void test_adj_regression_import_permission_com5003::scpiImportFailFlashWrite()
{
    setupServers(std::make_shared<MockAtmelCtrlFactory>(true));

    QString xmlFileName = ":/import_minimal_pass.xml";
    QString xml = XmlHelperForTest::loadXml(xmlFileName);
    QVERIFY(!xml.isEmpty());

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", xml);
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_adj_regression_import_permission_com5003::scpiImportPassFlashWrite()
{
    MockI2cEEpromIoFactory::enableMock();
    setupServers(std::make_shared<MockAtmelCtrlFactory>(true));

    QString xmlFileName = ":/import_minimal_pass.xml";
    QString xml = XmlHelperForTest::loadXml(xmlFileName);
    QVERIFY(!xml.isEmpty());

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", xml);
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_adj_regression_import_permission_com5003::setupServers(AtmelCtrlFactoryInterfacePrt ctrlFactory)
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceCom5003>(ctrlFactory);
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}
