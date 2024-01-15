#include "test_adj_import_invalid_eeprom_com5003.h"
#include "mockatmelctrlfactory.h"
#include "proxy.h"
#include "i2cflashiofactoryfortest.h"
#include "eeprom24lcmock.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_adj_import_invalid_eeprom_com5003);

static const QDateTime refTime = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);

void test_adj_import_invalid_eeprom_com5003::init()
{
    EEprom24LCMock::cleanAll();
    I2cFlashIoFactoryForTest::enableMockFlash();
    setupServers();
}

void test_adj_import_invalid_eeprom_com5003::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_adj_import_invalid_eeprom_com5003::importEmptyData()
{
    cI2CSettings *i2cSettings = m_mockServer->getI2cSettings();
    EEprom24LCMock eepromMock(i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_adj_import_invalid_eeprom_com5003::importIncompleteData()
{
    cI2CSettings *i2cSettings = m_mockServer->getI2cSettings();
    EEprom24LCMock eepromMock(i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));
    QByteArray eepromContent = readFile(":/export_internal_modified.eeprom");
    QVERIFY(!eepromContent.isEmpty());

    QList<int> notErrExec;
    for(int i=10; i<eepromContent.length(); i+=1000) {
        EEprom24LCMock::cleanAll();
        eepromMock.WriteData(eepromContent.left(i).data(), i, 0);
        QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
        if(ret != ZSCPI::scpiAnswer[ZSCPI::errexec])
            notErrExec.append(i);
    }
    for(int i=10; i<eepromContent.length(); i+=1000) {
        EEprom24LCMock::cleanAll();
        eepromMock.WriteData(eepromContent.left(i).data(), i, 0);
        eepromMock.returnReduceCountOnErrorRead();
        QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
        if(ret != ZSCPI::scpiAnswer[ZSCPI::errexec])
            notErrExec.append(i);
    }
    QVERIFY(notErrExec.isEmpty());
}

void test_adj_import_invalid_eeprom_com5003::setupServers()
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceCom5003>(std::make_shared<MockAtmelCtrlFactory>(true));
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

bool test_adj_import_invalid_eeprom_com5003::writeFile(QString filename, QByteArray data)
{
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly))
        return file.write(data) == data.length();
    return false;
}

QByteArray test_adj_import_invalid_eeprom_com5003::readFile(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
        return file.readAll();
    return QByteArray();
}