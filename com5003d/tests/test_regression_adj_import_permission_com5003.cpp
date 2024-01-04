#include "test_regression_adj_import_permission_com5003.h"
#include "proxy.h"
#include "atmel.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include "i2cflashiofactoryfortest.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_import_permission_com5003);

void test_regression_adj_import_permission_com5003::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_permission_com5003::scpiImportPermissionQueryFail()
{
    setupServers(&Atmel::getInstance());

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_regression_adj_import_permission_com5003::scpiImportNoPermission()
{
    AtmelPermissionTemplatePtrU perm = AtmelPermissionMock::createAlwaysDisabled();
    setupServers(perm.get());

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::erraut]);
}

void test_regression_adj_import_permission_com5003::scpiImportInvalidXml()
{
    AtmelPermissionTemplatePtrU perm = AtmelPermissionMock::createAlwaysEnabled();
    setupServers(perm.get());

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errxml]);
}

void test_regression_adj_import_permission_com5003::scpiImportFailFlashWrite()
{
    AtmelPermissionTemplatePtrU perm = AtmelPermissionMock::createAlwaysEnabled();
    setupServers(perm.get());

    QString xmlFileName = ":/import_minimal_pass.xml";
    QString xml = XmlHelperForTest::loadXml(xmlFileName);
    QVERIFY(!xml.isEmpty());

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", xml);
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_regression_adj_import_permission_com5003::scpiImportPassFlashWrite()
{
    I2cFlashIoFactoryForTest::enableMockFlash();
    AtmelPermissionTemplatePtrU perm = AtmelPermissionMock::createAlwaysEnabled();
    setupServers(perm.get());

    QString xmlFileName = ":/import_minimal_pass.xml";
    QString xml = XmlHelperForTest::loadXml(xmlFileName);
    QVERIFY(!xml.isEmpty());

    QString ret = ScpiSingleTransactionBlocked::cmdXmlParam("SYSTEM:ADJUSTMENT:XML", xml);
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_regression_adj_import_permission_com5003::setupServers(AtmelPermissionTemplate *permissionQueryHandler)
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceCom5003>(permissionQueryHandler);
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}
