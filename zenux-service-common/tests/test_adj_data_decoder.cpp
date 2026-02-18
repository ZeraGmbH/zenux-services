#include "test_adj_data_decoder.h"
#include "adjdatacompleteinternstream.h"
#include "adjustmenteepromreadwrite.h"
#include "mockeepromi2cfactory.h"
#include <QFile>
#include <QTest>

QTEST_MAIN(test_adj_data_decoder)

void test_adj_data_decoder::initTestCase()
{
    MockEepromI2cFactory eepromFactory;
    EepromI2cDeviceInterfacePtr adjMemory = eepromFactory.createEeprom({"", 0},
                                                                       AbstractEepromI2cDevice::capacity24LC256);
    AdjustmentEepromReadWrite eepromRw(std::move(adjMemory));
    m_flashSizeAllDevicesAtTheTimeOfWriting = eepromRw.getMaxSize();
}

void test_adj_data_decoder::denyMaxSizeExceed()
{
    QFile file(":/export_internal_initial_mt310s2.eeprom");
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray ba = file.readAll();
    
    AdjDataCompleteInternStream reader1(ba.size());
    AdjDataPtr data1 = reader1.decodeAdjBytes(ba);
    QVERIFY(!data1->isEmpty());
    
    AdjDataCompleteInternStream reader2(ba.size()-1);
    AdjDataPtr data2 = reader2.decodeAdjBytes(ba);
    QVERIFY(data2->isEmpty());
}

void test_adj_data_decoder::readServerVersionAndDeviceNameForMT()
{
    QFile file(":/export_internal_initial_mt310s2.eeprom");
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray ba = file.readAll();
    
    AdjDataCompleteInternStream reader(m_flashSizeAllDevicesAtTheTimeOfWriting);
    std::shared_ptr<AdjDataCompleteIntern> adjData = reader.decodeAdjBytes(ba);

    QCOMPARE(adjData->getAdjHeader().m_deviceName, "Unknown");
    QCOMPARE(adjData->getAdjHeader().m_serverVersion, "V1.01");
    QCOMPARE(adjData->getAdjHeader().m_serialNumber, "Unknown");
    QCOMPARE(adjData->getAdjHeader().m_deviceVersion, "DEVICE: Unknown;PCB: Unknown;LCA: Unknown;CTRL: Unknown");
    QCOMPARE(adjData->getAdjHeader().m_adjustmentDate, QDateTime::fromSecsSinceEpoch(0, Qt::UTC));
}

void test_adj_data_decoder::readServerVersionAndDeviceNameForCOM()
{
    QFile file(":/export_internal_initial_com5003.eeprom");
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray ba = file.readAll();
    
    AdjDataCompleteInternStream reader(m_flashSizeAllDevicesAtTheTimeOfWriting);
    std::shared_ptr<AdjDataCompleteIntern> adjData = reader.decodeAdjBytes(ba);

    QCOMPARE(adjData->getAdjHeader().m_deviceName, "Unknown");
    QCOMPARE(adjData->getAdjHeader().m_serverVersion, "V1.00");
    QCOMPARE(adjData->getAdjHeader().m_serialNumber, "Unknown");
    QCOMPARE(adjData->getAdjHeader().m_deviceVersion, "DEVICE: Unknown;PCB: Unknown;LCA: Unknown;CTRL: Unknown");
    QCOMPARE(adjData->getAdjHeader().m_adjustmentDate, QDateTime::fromSecsSinceEpoch(0, Qt::UTC));
}

void test_adj_data_decoder::checkChannelRangeAvailability()
{
    QFile file(":/export_internal_initial_com5003.eeprom");
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray ba = file.readAll();
    
    AdjDataCompleteInternStream reader(m_flashSizeAllDevicesAtTheTimeOfWriting);
    std::shared_ptr<AdjDataCompleteIntern> adjData = reader.decodeAdjBytes(ba);

    QVERIFY(adjData->isChannelRangeAvailable("m0", "480V"));
    QVERIFY(!adjData->isChannelRangeAvailable("m0", "48V"));
    QVERIFY(!adjData->isChannelRangeAvailable("m10", "480V"));
}
