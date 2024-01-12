#include "test_adj_deny_import_mt310s2.h"
#include "mockatmelctrlfactory.h"
#include "mt310s2systeminfomock.h"
#include "proxy.h"
#include "i2cflashiofactoryfortest.h"
#include "eeprom24lcmock.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_adj_deny_import_mt310s2);

void test_adj_deny_import_mt310s2::init()
{
    EEprom24LCMock::cleanAll();
    I2cFlashIoFactoryForTest::enableMockFlash();
    setupServers();
}

void test_adj_deny_import_mt310s2::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_adj_deny_import_mt310s2::loadEEpromWithStoredNamesAndVersions()
{
    // This is mostly to set-up our mock SystemInfo
    cI2CSettings *i2cSettings = m_mockServer->getI2cSettings();
    EEprom24LCMock eepromMock(i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));
    QByteArray eepromContent = readFile(":/export_internal_modified.eeprom");
    QVERIFY(!eepromContent.isEmpty());
    eepromMock.WriteData(eepromContent.data(), eepromContent.length(), 0);

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

void test_adj_deny_import_mt310s2::loadEEpromAndDenyDifferentDeviceName()
{
    static_cast<Mt310s2SystemInfoMock*>(m_mockServer->getSystemInfo())->setDeviceName("Foo");

    cI2CSettings *i2cSettings = m_mockServer->getI2cSettings();
    EEprom24LCMock eepromMock(i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));
    QByteArray eepromContent = readFile(":/export_internal_modified.eeprom");
    QVERIFY(!eepromContent.isEmpty());
    eepromMock.WriteData(eepromContent.data(), eepromContent.length(), 0);

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_adj_deny_import_mt310s2::setupServers()
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceMt310s2>(std::make_shared<MockAtmelCtrlFactory>(true),
                                                                  true);
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

QByteArray test_adj_deny_import_mt310s2::readFile(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
        return file.readAll();
    return QByteArray();
}
