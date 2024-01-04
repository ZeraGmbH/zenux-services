#include "test_regression_adj_import_export_flash_com5003.h"
#include "proxy.h"
#include "i2cflashiofactoryfortest.h"
#include "flash24lc256mock.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_import_export_flash_com5003);

static const QDateTime refTime = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);

void test_regression_adj_import_export_flash_com5003::initTestCase()
{
    // permission tests are done in test_regression_adj_import_export_xml_<device>
    m_permissionMock = AtmelPermissionMock::createAlwaysEnabled();
}

void test_regression_adj_import_export_flash_com5003::init()
{
    Flash24LC256Mock::cleanAll();
    I2cFlashIoFactoryForTest::enableMockFlash();
    setupServers(m_permissionMock.get());
}

void test_regression_adj_import_export_flash_com5003::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_export_flash_com5003::directExportFlashNoMock()
{
    I2cFlashIoFactoryForTest::disableMockFlash();
    QVERIFY(!m_mockServer->getAdjustment()->exportAdjFlash(refTime));
}

void test_regression_adj_import_export_flash_com5003::directExportFlashGen()
{
    QVERIFY(m_mockServer->getAdjustment()->exportAdjFlash(refTime));
    cI2CSettings *i2cSettings = m_mockServer->getI2cSettings();
    QByteArray dataWritten = Flash24LC256Mock::getData(i2cSettings->getDeviceNode(),
                                                       i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));
    QVERIFY(!dataWritten.isEmpty());
    QVERIFY(writeFile("/tmp/export_internal_initial.eeprom", dataWritten));
}

void test_regression_adj_import_export_flash_com5003::directExportFlashCheckReference()
{
    QVERIFY(m_mockServer->getAdjustment()->exportAdjFlash(refTime));
    cI2CSettings *i2cSettings = m_mockServer->getI2cSettings();
    QByteArray dataWritten = Flash24LC256Mock::getData(i2cSettings->getDeviceNode(),
                                                       i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));
    QByteArray dataReference = readFile(":/export_internal_initial.eeprom");
    QCOMPARE(dataWritten, dataReference);
}

void test_regression_adj_import_export_flash_com5003::scpiWriteFlashInitial()
{
    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:WRITE", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    cI2CSettings *i2cSettings = m_mockServer->getI2cSettings();
    QString devNode = i2cSettings->getDeviceNode();
    short i2cAddress = i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress);

    // SCPI write flash sets current date so we cannot compare it
    // Check if at least a write happened
    QCOMPARE(Flash24LC256Mock::getWriteCount(devNode, i2cAddress), 1);

    // and do a second write with known time
    QVERIFY(m_mockServer->getAdjustment()->exportAdjFlash(refTime));
    QCOMPARE(Flash24LC256Mock::getWriteCount(devNode, i2cAddress), 2);
    QByteArray dataWritten = Flash24LC256Mock::getData(devNode, i2cAddress);
    QByteArray dataReference = readFile(":/export_internal_initial.eeprom");
    QCOMPARE(dataWritten, dataReference);
}

void test_regression_adj_import_export_flash_com5003::setupServers(AtmelPermissionTemplate *permissionQueryHandler)
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

bool test_regression_adj_import_export_flash_com5003::writeFile(QString filename, QByteArray data)
{
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly))
        return file.write(data) == data.length();
    return false;
}

QByteArray test_regression_adj_import_export_flash_com5003::readFile(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
        return file.readAll();
    return QByteArray();
}
