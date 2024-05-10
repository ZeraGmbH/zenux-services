#include "test_regression_adj_import_export_eeprom_mt310s2.h"
#include "clampfactorytest.h"
#include "testfactoryi2cctrl.h"
#include "testfactoryi2cctrlcommoninfofoo.h"
#include "proxy.h"
#include "mocki2ceepromiofactory.h"
#include "mockeeprom24lc.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_import_export_eeprom_mt310s2);

static const QDateTime refTime = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);

void test_regression_adj_import_export_eeprom_mt310s2::initTestCase()
{
    ClampFactoryTest::enableTest();
}

void test_regression_adj_import_export_eeprom_mt310s2::init()
{
    MockEEprom24LC::mockCleanAll();
    MockI2cEEpromIoFactory::enableMock();
}

void test_regression_adj_import_export_eeprom_mt310s2::cleanup()
{
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_export_eeprom_mt310s2::directExportFlashNoMock()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    MockI2cEEpromIoFactory::disableMock();
    QVERIFY(!m_testServer->getSenseInterface()->exportAdjData(refTime));
}

void test_regression_adj_import_export_eeprom_mt310s2::directExportFlashGen()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QVERIFY(MockEEprom24LC::mockWriteToFile(i2cSettings->getDeviceNode(),
                                            i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                                            "/tmp/export_internal_initial.eeprom"));
}

void test_regression_adj_import_export_eeprom_mt310s2::directExportFlashCheckReference()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QVERIFY(MockEEprom24LC::mockCompareWithFile(i2cSettings->getDeviceNode(),
                                                i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                                                ":/export_internal_initial.eeprom"));
}

void test_regression_adj_import_export_eeprom_mt310s2::scpiWriteFlashInitial()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:WRITE", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QString devNode = i2cSettings->getDeviceNode();
    short i2cAddress = i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress);

    // SCPI write flash sets current date so we cannot compare it
    // Check if at least a write happened
    QCOMPARE(MockEEprom24LC::mockGetWriteCount(devNode, i2cAddress), 1);

    // and do a second write with known time
    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
    QCOMPARE(MockEEprom24LC::mockGetWriteCount(devNode, i2cAddress), 2);
    QVERIFY(MockEEprom24LC::mockCompareWithFile(devNode, i2cAddress, ":/export_internal_initial.eeprom"));
}

void test_regression_adj_import_export_eeprom_mt310s2::scpiWriteRandomFileAndFlashGen()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QString filenameShort = ":/import_modified";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:XML:READ", filenameShort);
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
    ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:WRITE", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QVERIFY(MockEEprom24LC::mockWriteToFile(i2cSettings->getDeviceNode(),
                                            i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                                            "/tmp/export_internal_modified.eeprom"));
}

void test_regression_adj_import_export_eeprom_mt310s2::scpiWriteRandomFileFlashWriteFlashReadExportXmlAndCheck()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QString filenameShort = ":/import_modified";
    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:XML:READ", filenameShort);
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
    ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:WRITE", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
    ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString xmlExported = XmlHelperForTest::prepareForCompare(ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?"));

    QFile xmlFile(":/import_modified.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = XmlHelperForTest::prepareForCompare(xmlFile.readAll());

    qInfo("Exported XML:");
    qInfo("%s", qPrintable(xmlExported));
    qInfo("Expected XML:");
    qInfo("%s", qPrintable(xmlExpected));
    QCOMPARE(xmlExported, xmlExpected);
}

void test_regression_adj_import_export_eeprom_mt310s2::loadRandomToEEpromWriteToFlashExportXmlAndCheck()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QVERIFY(MockEEprom24LC::mockReadFromFile(i2cSettings->getDeviceNode(),
                                             i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                                             ":/export_internal_modified.eeprom"));

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString xmlExported = XmlHelperForTest::prepareForCompare(ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?"));

    QFile xmlFile(":/import_modified.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = XmlHelperForTest::prepareForCompare(xmlFile.readAll());

    qInfo("Exported XML:");
    qInfo("%s", qPrintable(xmlExported));
    qInfo("Expected XML:");
    qInfo("%s", qPrintable(xmlExpected));
    QCOMPARE(xmlExported, xmlExpected);
}

void test_regression_adj_import_export_eeprom_mt310s2::directExportFlashArbitraryVersionGen()
{
    setupServers(std::make_shared<TestFactoryI2cCtrlCommonInfoFoo>());
    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QVERIFY(MockEEprom24LC::mockWriteToFile(i2cSettings->getDeviceNode(),
                                            i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                                            "/tmp/import_arbitrary_version.eeprom"));
}

void test_regression_adj_import_export_eeprom_mt310s2::loadArbitraryVersionToEEprom()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QVERIFY(MockEEprom24LC::mockReadFromFile(i2cSettings->getDeviceNode(),
                                             i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                                             ":/import_arbitrary_version.eeprom"));

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_regression_adj_import_export_eeprom_mt310s2::setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(ctrlFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
