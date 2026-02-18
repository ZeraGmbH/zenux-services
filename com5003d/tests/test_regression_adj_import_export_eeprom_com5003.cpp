#include "test_regression_adj_import_export_eeprom_com5003.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "mockserverparamgenerator.h"
#include "mockeepromdevice.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <mocktcpnetworkfactory.h>
#include <QTimeZone>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_import_export_eeprom_com5003);

static const QDateTime refTime = QDateTime::fromSecsSinceEpoch(0, QTimeZone::utc());

void test_regression_adj_import_export_eeprom_com5003::cleanup()
{
    MockEepromDevice::cleanAll();
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_export_eeprom_com5003::directExportFlashError()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    MockEepromDevice::setGlobalError(true);
    QVERIFY(!m_testServer->getSenseInterface()->exportAdjData(refTime));
}

void test_regression_adj_import_export_eeprom_com5003::directExportFlashGen()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
    I2cSettingsPtr i2cSettings = m_testServer->getI2cSettings();
    TestLogHelpers::writeFile("/tmp/export_internal_initial.eeprom",
                              MockEepromDevice::getData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)}));
}

void test_regression_adj_import_export_eeprom_com5003::directExportFlashCheckReference()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
    I2cSettingsPtr i2cSettings = m_testServer->getI2cSettings();
    QByteArray expected = TestLogHelpers::loadFile(":/export_internal_initial.eeprom");
    QByteArray dumped = MockEepromDevice::getData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)});
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, dumped));
}

void test_regression_adj_import_export_eeprom_com5003::scpiWriteFlashInitial()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:WRITE", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    I2cSettingsPtr i2cSettings = m_testServer->getI2cSettings();
    QString devNodeFileName = i2cSettings->getDeviceNode();
    const ushort i2cAddress = i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress);

    // SCPI write flash sets current date so we cannot compare it
    // Check if at least a write happened
    QCOMPARE(MockEepromDevice::getWriteCount({devNodeFileName, i2cAddress}), 1);

    // and do a second write with known time
    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
    QCOMPARE(MockEepromDevice::getWriteCount({devNodeFileName, i2cAddress}), 2);
    QByteArray expected = TestLogHelpers::loadFile(":/export_internal_initial.eeprom");
    QByteArray dumped = MockEepromDevice::getData({devNodeFileName, i2cAddress});
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, dumped));
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

    I2cSettingsPtr i2cSettings = m_testServer->getI2cSettings();
    TestLogHelpers::writeFile("/tmp/export_internal_modified_with_date_time.eeprom",
                              MockEepromDevice::getData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)}));
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
    QString xmlExpected = TestLogHelpers::loadFile(":/import_modified.xml");
    xmlExpected = XmlHelperForTest::prepareForCompare(xmlExpected);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_regression_adj_import_export_eeprom_com5003::loadOriginalInvalidDateTimeRandomToEEpromWriteToFlashExportXmlAndCheck()
{
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("com5003d"));
    I2cSettingsPtr i2cSettings = settings->getI2cSettings();
    MockEepromDevice::setData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)},
                              TestLogHelpers::loadFile(":/export_internal_modified.eeprom"));
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString xmlExported = XmlHelperForTest::prepareForCompare(ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?"));
    QString xmlExpected = TestLogHelpers::loadFile(":/import_modified.xml");
    xmlExpected = XmlHelperForTest::prepareForCompare(xmlExpected);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_regression_adj_import_export_eeprom_com5003::loadValidDateTimeRandomToEEpromWriteToFlashExportXmlAndCheck()
{
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("com5003d"));
    I2cSettingsPtr i2cSettings = settings->getI2cSettings();
    MockEepromDevice::setData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)},
                              TestLogHelpers::loadFile(":/export_internal_modified_with_date_time.eeprom"));
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString xmlExported = XmlHelperForTest::prepareForCompare(ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?"));
    QString xmlExpected = TestLogHelpers::loadFile(":/import_modified.xml");
    xmlExpected = XmlHelperForTest::prepareForCompare(xmlExpected);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_regression_adj_import_export_eeprom_com5003::directExportFlashArbitraryVersionGen()
{
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("com5003d"));
    I2cSettingsPtr i2cSettings = settings->getI2cSettings();
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true, "foo"));

    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
    TestLogHelpers::writeFile("/tmp/import_arbitrary_version.eeprom",
                              MockEepromDevice::getData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)}));
    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
}

void test_regression_adj_import_export_eeprom_com5003::loadArbitraryVersionToEEprom()
{
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("com5003d"));
    I2cSettingsPtr i2cSettings = settings->getI2cSettings();
    MockEepromDevice::setData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)},
                              TestLogHelpers::loadFile(":/import_arbitrary_version.eeprom"));
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_regression_adj_import_export_eeprom_com5003::setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resmanServer = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceCom5003>(ctrlFactory, tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
