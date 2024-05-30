#include "test_regression_adj_import_export_eeprom_com5003.h"
#include "testfactoryi2cctrl.h"
#include "testfactoryi2cctrlcommoninfofoo.h"
#include "proxy.h"
#include "mockserverparamgenerator.h"
#include "mocki2ceepromiofactory.h"
#include "mockeeprom24lc.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_import_export_eeprom_com5003);

static const QDateTime refTime = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);

void test_regression_adj_import_export_eeprom_com5003::init()
{
    MockEEprom24LC::mockCleanAll();
    MockI2cEEpromIoFactory::enableMock();
}

void test_regression_adj_import_export_eeprom_com5003::cleanup()
{
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_export_eeprom_com5003::directExportFlashNoMock()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    MockI2cEEpromIoFactory::disableMock();
    QVERIFY(!m_testServer->getSenseInterface()->exportAdjData(refTime));
}

void test_regression_adj_import_export_eeprom_com5003::directExportFlashGen()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QVERIFY(MockEEprom24LC::mockWriteToFile(i2cSettings->getDeviceNode(),
                                            i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                                            "/tmp/export_internal_initial.eeprom"));
}

void test_regression_adj_import_export_eeprom_com5003::directExportFlashCheckReference()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QVERIFY(MockEEprom24LC::mockCompareWithFile(i2cSettings->getDeviceNode(),
                                                i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                                                ":/export_internal_initial.eeprom"));
}

void test_regression_adj_import_export_eeprom_com5003::scpiWriteFlashInitial()
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

void test_regression_adj_import_export_eeprom_com5003::scpiWriteRandomFileAndFlashGen()
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
                                            "/tmp/export_internal_modified_with_date_time.eeprom"));
}

void test_regression_adj_import_export_eeprom_com5003::scpiWriteRandomFileFlashWriteFlashReadExportXmlAndCheck()
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

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_regression_adj_import_export_eeprom_com5003::loadOriginalInvalidDateTimeRandomToEEpromWriteToFlashExportXmlAndCheck()
{
    std::unique_ptr<SettingsContainer> settings =  std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("com5003d"));
    I2cSettings *i2cSettings = settings->getI2cSettings();
    QVERIFY(MockEEprom24LC::mockReadFromFile(i2cSettings->getDeviceNode(),
                                             i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                                             ":/export_internal_modified.eeprom"));
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString xmlExported = XmlHelperForTest::prepareForCompare(ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?"));

    QFile xmlFile(":/import_modified.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = XmlHelperForTest::prepareForCompare(xmlFile.readAll());

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_regression_adj_import_export_eeprom_com5003::loadValidDateTimeRandomToEEpromWriteToFlashExportXmlAndCheck()
{
    std::unique_ptr<SettingsContainer> settings =  std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("com5003d"));
    I2cSettings *i2cSettings = settings->getI2cSettings();
    QVERIFY(MockEEprom24LC::mockReadFromFile(i2cSettings->getDeviceNode(),
                                             i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                                             ":/export_internal_modified_with_date_time.eeprom"));
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString xmlExported = XmlHelperForTest::prepareForCompare(ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?"));

    QFile xmlFile(":/import_modified.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = XmlHelperForTest::prepareForCompare(xmlFile.readAll());

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_regression_adj_import_export_eeprom_com5003::directExportFlashArbitraryVersionGen()
{
    std::unique_ptr<SettingsContainer> settings =  std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("com5003d"));
    I2cSettings *i2cSettings = settings->getI2cSettings();
    setupServers(std::make_shared<TestFactoryI2cCtrlCommonInfoFoo>());

    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
    QVERIFY(MockEEprom24LC::mockWriteToFile(i2cSettings->getDeviceNode(),
                                            i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                                            "/tmp/import_arbitrary_version.eeprom"));
    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
}

void test_regression_adj_import_export_eeprom_com5003::loadArbitraryVersionToEEprom()
{
    std::unique_ptr<SettingsContainer> settings =  std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("com5003d"));
    I2cSettings *i2cSettings = settings->getI2cSettings();
    QVERIFY(MockEEprom24LC::mockReadFromFile(i2cSettings->getDeviceNode(),
                                             i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                                             ":/import_arbitrary_version.eeprom"));
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_regression_adj_import_export_eeprom_com5003::setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_testServer = std::make_unique<TestServerForSenseInterfaceCom5003>(ctrlFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
