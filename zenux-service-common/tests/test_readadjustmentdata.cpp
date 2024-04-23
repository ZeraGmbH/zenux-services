#include "test_readadjustmentdata.h"
#include "adjustmenteepromdatareader.h"
#include <QDataStream>
#include <QFile>
#include <QTest>

QTEST_MAIN(test_readadjustmentdata)

void test_readadjustmentdata::checkFileNotEmpty()
{
    AdjustmentEepromDataReader reader;
    QFile file(":/export_internal_initial_mt310s2.eeprom");
    QVERIFY(file.open(QIODevice::ReadOnly));
    QDataStream stream(&file);
    QVERIFY(reader.ignoreCountAndCheckSum(stream));
}

void test_readadjustmentdata::readServerVersionAndDeviceNameForMT()
{
    AdjustmentEepromDataReader reader;
    QFile file(":/export_internal_initial_mt310s2.eeprom");
    file.open(QIODevice::ReadOnly);
    QDataStream stream(&file);
    QVERIFY(reader.extractDeviceInfos(stream));
    QCOMPARE(reader.getServerVersion(), "V1.01");
    QCOMPARE(reader.getDeviceName(), "Unknown");
}

void test_readadjustmentdata::readMT310s2Ranges()
{
    QFile file(":/export_internal_initial_mt310s2.eeprom");
    file.open(QIODevice::ReadOnly);
    QDataStream stream(&file);
    AdjustmentEepromDataReader reader(stream);

    QStringList rangesList = reader.getRangesList();
    QCOMPARE(rangesList.size(), 88);
    QVERIFY(rangesList.contains("250V"));
    QVERIFY(rangesList.contains("10A"));
    QVERIFY(!rangesList.contains("480V"));
}

void test_readadjustmentdata::readServerVersionAndDeviceNameForCOM()
{
    AdjustmentEepromDataReader reader;
    QFile file(":/export_internal_initial_com5003.eeprom");
    file.open(QIODevice::ReadOnly);
    QDataStream stream(&file);
    QVERIFY(reader.extractDeviceInfos(stream));
    QCOMPARE(reader.getServerVersion(), "V1.00");
    QCOMPARE(reader.getDeviceName(), "Unknown");
}

void test_readadjustmentdata::readCOM5003Ranges()
{
    QFile file(":/export_internal_initial_com5003.eeprom");
    file.open(QIODevice::ReadOnly);
    QDataStream stream(&file);
    AdjustmentEepromDataReader reader(stream);

    QStringList rangesList = reader.getRangesList();
    QCOMPARE(rangesList.size(), 75);
    QVERIFY(rangesList.contains("480V"));
    QVERIFY(rangesList.contains("10A"));
    QVERIFY(!rangesList.contains("250V"));
}


