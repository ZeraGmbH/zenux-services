#include "test_adj_regression_import_export_eeprom_com5003.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "mocki2ceepromiofactory.h"
#include "mockeeprom24lc.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_adj_regression_import_export_eeprom_com5003);

static const QDateTime refTime = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);

void test_adj_regression_import_export_eeprom_com5003::init()
{
    MockEEprom24LC::cleanAll();
    MockI2cEEpromIoFactory::enableMock();
}

void test_adj_regression_import_export_eeprom_com5003::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_adj_regression_import_export_eeprom_com5003::directExportFlashNoMock()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    MockI2cEEpromIoFactory::disableMock();
    QVERIFY(!m_testServer->getSenseInterface()->exportAdjFlash(refTime));
}

void test_adj_regression_import_export_eeprom_com5003::directExportFlashGen()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QVERIFY(m_testServer->getSenseInterface()->exportAdjFlash(refTime));
    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QByteArray dataWritten = MockEEprom24LC::getData(i2cSettings->getDeviceNode(),
                                                       i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));
    QVERIFY(!dataWritten.isEmpty());
    QVERIFY(writeFile("/tmp/export_internal_initial.eeprom", dataWritten));
}

void test_adj_regression_import_export_eeprom_com5003::directExportFlashCheckReference()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QVERIFY(m_testServer->getSenseInterface()->exportAdjFlash(refTime));
    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QByteArray dataWritten = MockEEprom24LC::getData(i2cSettings->getDeviceNode(),
                                                       i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));
    QByteArray dataReference = readFile(":/export_internal_initial.eeprom");
    QCOMPARE(dataWritten, dataReference);
}

void test_adj_regression_import_export_eeprom_com5003::scpiWriteFlashInitial()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:WRITE", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QString devNode = i2cSettings->getDeviceNode();
    short i2cAddress = i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress);

    // SCPI write flash sets current date so we cannot compare it
    // Check if at least a write happened
    QCOMPARE(MockEEprom24LC::getWriteCount(devNode, i2cAddress), 1);

    // and do a second write with known time
    QVERIFY(m_testServer->getSenseInterface()->exportAdjFlash(refTime));
    QCOMPARE(MockEEprom24LC::getWriteCount(devNode, i2cAddress), 2);
    QByteArray dataWritten = MockEEprom24LC::getData(devNode, i2cAddress);
    QByteArray dataReference = readFile(":/export_internal_initial.eeprom");
    QCOMPARE(dataWritten, dataReference);
}

void test_adj_regression_import_export_eeprom_com5003::scpiWriteRandomFileAndFlashGen()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QString filenameShort = ":/import_modified";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:XML:READ", filenameShort);
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
    ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:WRITE", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QByteArray dataWritten = MockEEprom24LC::getData(i2cSettings->getDeviceNode(),
                                                       i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));
    QVERIFY(!dataWritten.isEmpty());
    QVERIFY(writeFile("/tmp/export_internal_modified_with_date_time.eeprom", dataWritten));
}

void test_adj_regression_import_export_eeprom_com5003::scpiWriteRandomFileFlashWriteFlashReadExportXmlAndCheck()
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

void test_adj_regression_import_export_eeprom_com5003::loadOriginalInvalidDateTimeRandomToEEpromWriteToFlashExportXmlAndCheck()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    MockEEprom24LC eepromMock(i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));
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

void test_adj_regression_import_export_eeprom_com5003::loadValidDateTimeRandomToEEpromWriteToFlashExportXmlAndCheck()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    MockEEprom24LC eepromMock(i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));
    QByteArray eepromContent = readFile(":/export_internal_modified_with_date_time.eeprom");
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

void test_adj_regression_import_export_eeprom_com5003::setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_testServer = std::make_unique<TestServerForSenseInterfaceCom5003>(ctrlFactory);
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

bool test_adj_regression_import_export_eeprom_com5003::writeFile(QString filename, QByteArray data)
{
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly))
        return file.write(data) == data.length();
    return false;
}

QByteArray test_adj_regression_import_export_eeprom_com5003::readFile(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
        return file.readAll();
    return QByteArray();
}
