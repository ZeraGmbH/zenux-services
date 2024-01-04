#include "test_regression_adj_import_export_flash_mt310s2.h"
#include "clampfactorytest.h"
#include "proxy.h"
#include "atmel.h"
#include "i2cflashiofactoryfortest.h"
#include "flash24lc256mock.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_import_export_flash_mt310s2);

static const QDateTime refTime = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);

void test_regression_adj_import_export_flash_mt310s2::initTestCase()
{
    ClampFactoryTest::enableTest();
}

void test_regression_adj_import_export_flash_mt310s2::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_export_flash_mt310s2::directExportFlashNoMock()
{
    I2cFlashIoFactoryForTest::disableMockFlash();
    setupServers(&Atmel::getInstance());

    QVERIFY(!m_mockServer->getSenseInterface()->exportAdjFlash(refTime));
}

void test_regression_adj_import_export_flash_mt310s2::directExportFlash()
{
    I2cFlashIoFactoryForTest::enableMockFlash();
    Flash24LC256Mock::cleanAll();
    setupServers(&Atmel::getInstance());

    QVERIFY(m_mockServer->getSenseInterface()->exportAdjFlash(refTime));
    cI2CSettings *i2cSettings = m_mockServer->getI2cSettings();
    QByteArray dataWritten = Flash24LC256Mock::getData(i2cSettings->getDeviceNode(),
                                                       i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));
    QVERIFY(!dataWritten.isEmpty());
    QVERIFY(writeFile("/tmp/export_flash_internal_initial", dataWritten));

    QByteArray dataReference = readFile(":/export_flash_internal_initial");
    QCOMPARE(dataWritten, dataReference);
}

void test_regression_adj_import_export_flash_mt310s2::setupServers(AtmelPermissionTemplate *permissionQueryHandler)
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceMt310s2>(permissionQueryHandler);
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

bool test_regression_adj_import_export_flash_mt310s2::writeFile(QString filename, QByteArray data)
{
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly))
        return file.write(data) == data.length();
    return false;
}

QByteArray test_regression_adj_import_export_flash_mt310s2::readFile(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
        return file.readAll();
    return QByteArray();
}
