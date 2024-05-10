#include "test_adj_import_invalid_eeprom_com5003.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "mocki2ceepromiofactory.h"
#include "mockeeprom24lc.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_adj_import_invalid_eeprom_com5003);

static const QDateTime refTime = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);

void test_adj_import_invalid_eeprom_com5003::init()
{
    MockEEprom24LC::mockCleanAll();
    MockI2cEEpromIoFactory::enableMock();
    setupServers();
}

void test_adj_import_invalid_eeprom_com5003::cleanup()
{
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_adj_import_invalid_eeprom_com5003::importEmptyData()
{
    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    MockEEprom24LC eepromMock(i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_adj_import_invalid_eeprom_com5003::importIncompleteData()
{
    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    MockEEprom24LC eepromMock(i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));
    QByteArray eepromContent = readFile(":/export_internal_modified.eeprom");
    QVERIFY(!eepromContent.isEmpty());

    QList<int> notErrExec;
    for(int i=10; i<eepromContent.length(); i+=1000) {
        MockEEprom24LC::mockCleanAll();
        eepromMock.WriteData(eepromContent.left(i).data(), i, 0);
        QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
        if(ret != ZSCPI::scpiAnswer[ZSCPI::errexec])
            notErrExec.append(i);
    }
    for(int i=10; i<eepromContent.length(); i+=1000) {
        MockEEprom24LC::mockCleanAll();
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
    m_testServer = std::make_unique<TestServerForSenseInterfaceCom5003>(std::make_shared<TestFactoryI2cCtrl>(true));
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

QByteArray test_adj_import_invalid_eeprom_com5003::readFile(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
        return file.readAll();
    return QByteArray();
}
