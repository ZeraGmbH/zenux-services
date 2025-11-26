#include "test_regression_adj_import_export_eeprom_mt310s2.h"
#include "clamp.h"
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
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_import_export_eeprom_mt310s2);

static const QDateTime refTime = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);

void test_regression_adj_import_export_eeprom_mt310s2::cleanup()
{
    MockEepromDevice::cleanAll();
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_export_eeprom_mt310s2::directExportFlashError()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    MockEepromDevice::setGlobalError(true);
    QVERIFY(!m_testServer->getSenseInterface()->exportAdjData(refTime));
}

void test_regression_adj_import_export_eeprom_mt310s2::directExportFlashGen()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
    const I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    TestLogHelpers::writeFile("/tmp/export_internal_initial.eeprom",
                              MockEepromDevice::getData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)}));
}

void test_regression_adj_import_export_eeprom_mt310s2::directExportFlashCheckReference()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
    const I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QByteArray expected = TestLogHelpers::loadFile(":/export_internal_initial.eeprom");
    QByteArray dumped = MockEepromDevice::getData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)});
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, dumped));
}

void test_regression_adj_import_export_eeprom_mt310s2::scpiWriteFlashInitial()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:WRITE", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    const I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QString devNodeFileName = i2cSettings->getDeviceNode();
    ushort i2cAddress = i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress);

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

void test_regression_adj_import_export_eeprom_mt310s2::scpiWriteRandomFileAndFlashGen()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QString filenameShort = ":/import_modified";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:XML:READ", filenameShort);
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
    ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:WRITE", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    const I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    TestLogHelpers::writeFile("/tmp/export_internal_modified.eeprom",
                              MockEepromDevice::getData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)}));
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

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_regression_adj_import_export_eeprom_mt310s2::loadRandomToEEpromWriteToFlashExportXmlAndCheck()
{
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("mt310s2d"));
    const I2cSettings *i2cSettings = settings->getI2cSettings();
    MockEepromDevice::setData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)},
                              TestLogHelpers::loadFile(":/export_internal_modified.eeprom"));
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString xmlExported = XmlHelperForTest::prepareForCompare(ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?"));

    QFile xmlFile(":/import_modified.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = XmlHelperForTest::prepareForCompare(xmlFile.readAll());

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_regression_adj_import_export_eeprom_mt310s2::directExportFlashArbitraryVersionGen()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true, "foo"));
    QVERIFY(m_testServer->getSenseInterface()->exportAdjData(refTime));
    const I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    TestLogHelpers::writeFile("/tmp/import_arbitrary_version.eeprom",
                              MockEepromDevice::getData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)}));
}

void test_regression_adj_import_export_eeprom_mt310s2::loadArbitraryVersionToEEprom()
{
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("mt310s2d"));
    const I2cSettings *i2cSettings = settings->getI2cSettings();
    MockEepromDevice::setData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)},
                              TestLogHelpers::loadFile(":/import_arbitrary_version.eeprom"));
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_regression_adj_import_export_eeprom_mt310s2::allClampsEEpromWrite_data()
{
    QTest::addColumn<int>("clampType");
    QTest::addColumn<QString>("clampName");
    for (int clampType=cClamp::undefined+1; clampType<cClamp::anzCL; ++clampType) {
        const QString clampName = cClamp::getClampTypeName(clampType);
        QTest::newRow(qPrintable(clampName)) << clampType << clampName;
    }
}

void test_regression_adj_import_export_eeprom_mt310s2::allClampsEEpromWrite()
{
    QFETCH(int, clampType);
    QFETCH(QString, clampName);

    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    cClamp *clamp = m_testServer->addClamp(0, "IAUX");

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:CLAMP:m7:TYPE", QString::number(clampType));
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
    QVERIFY(clamp->exportClampAdjData(refTime));

    const I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    const QByteArray eepromData = MockEepromDevice::getData(
        {i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::clampFlashI2cAddress)},
        {i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::muxerI2cAddress)},
        clamp->getChannelSettings()->m_nMuxChannelNo);
    QVERIFY(TestLogHelpers::writeFile("/tmp/clamps/fresh/" + clampName + ".eeprom", eepromData));
}

void test_regression_adj_import_export_eeprom_mt310s2::freshClampSetTypeIOnlyEepromCheck()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    cClamp *clamp = m_testServer->addClamp(0, "IAUX");

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:CLAMP:m7:TYPE", QString::number(cClamp::CL120A));
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
    QVERIFY(clamp->exportClampAdjData(refTime));

    const I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QByteArray expected = TestLogHelpers::loadFile(":/initial_clamp_type_i.eeprom");
    QByteArray dumped = MockEepromDevice::getData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::clampFlashI2cAddress)},
                                                  {i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::muxerI2cAddress)},
                                                  clamp->getChannelSettings()->m_nMuxChannelNo);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, dumped));
}

void test_regression_adj_import_export_eeprom_mt310s2::freshClampSetTypeUOnlyEepromCheck()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    cClamp *clamp = m_testServer->addClamp(0, "IAUX");

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:CLAMP:m7:TYPE", QString::number(cClamp::CL1000VDC));
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
    QVERIFY(clamp->exportClampAdjData(refTime));

    const I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QByteArray expected = TestLogHelpers::loadFile(":/initial_clamp_type_u.eeprom");
    QByteArray dumped = MockEepromDevice::getData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::clampFlashI2cAddress)},
                                                  {i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::muxerI2cAddress)},
                                                  clamp->getChannelSettings()->m_nMuxChannelNo);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, dumped));
}

void test_regression_adj_import_export_eeprom_mt310s2::freshClampSetTypeUIEepromCheck()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    cClamp *clamp = m_testServer->addClamp(0, "IAUX");

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:CLAMP:m7:TYPE", QString::number(cClamp::CL200ADC1000VDC));
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
    QVERIFY(clamp->exportClampAdjData(refTime));

    const I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QByteArray expected = TestLogHelpers::loadFile(":/initial_clamp_type_ui.eeprom");
    QByteArray dumped = MockEepromDevice::getData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::clampFlashI2cAddress)},
                                                  {i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::muxerI2cAddress)},
                                                  clamp->getChannelSettings()->m_nMuxChannelNo);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, dumped));
}

void test_regression_adj_import_export_eeprom_mt310s2::setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resmanServer = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(ctrlFactory, tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
