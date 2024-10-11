#include "test_disk_read_write.h"
#include "procdiskstatdecoder.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_disk_read_write)

void test_disk_read_write::allDiskBlockDevices()
{
    TestSystemInfoFileLocator::setProcDiskStatsName(":/procDisksBlockDevices");
    QStringList devices = ProcDiskStatDecoder::getAllDiskBlockDevices();
    QCOMPARE(devices.count(), 17);
}

void test_disk_read_write::diskBlockDevicesOfInterest()
{
    TestSystemInfoFileLocator::setProcDiskStatsName(":/procDisksBlockDevices");
    QStringList devices = ProcDiskStatDecoder::getDiskBlockDevicesOfInterest();
    QCOMPARE(devices.count(), 5);
    QVERIFY(devices.contains("sda"));
    QVERIFY(devices.contains("sdb"));
    QVERIFY(devices.contains("sdc"));
    QVERIFY(devices.contains("mmcblk0"));
    QVERIFY(devices.contains("mmcblk1"));
}

constexpr quint64 sectorSize = 512; // see comments in ProcDiskStatDecoder::decodeSingleDiskStatLine

void test_disk_read_write::decodeLine()
{
    DiskValues values;
    values = ProcDiskStatDecoder::decodeSingleDiskStatLine("0      0 mmcblk1 0 0 1 0 0 0 2 0 0 0 0 0 0 0 0");
    QCOMPARE(values.totalReadBytes, 1*sectorSize);
    QCOMPARE(values.totalWriteBytes, 2*sectorSize);
    values = ProcDiskStatDecoder::decodeSingleDiskStatLine("0      0 mmcblk1 0 0 3 0 0 0 4 0 0 0 0 0 0 0 0");
    QCOMPARE(values.totalReadBytes, 3*sectorSize);
    QCOMPARE(values.totalWriteBytes, 4*sectorSize);
}

void test_disk_read_write::decodeInvalidLines()
{
    DiskValues values;
    values = ProcDiskStatDecoder::decodeSingleDiskStatLine("0      0 mmcblk1 0 0 a 0 0 0 b 0 0 0 0 0 0 0 0");
    QCOMPARE(values.totalReadBytes, 0);
    QCOMPARE(values.totalWriteBytes, 0);
    values = ProcDiskStatDecoder::decodeSingleDiskStatLine("");
    QCOMPARE(values.totalReadBytes, 0);
    QCOMPARE(values.totalWriteBytes, 0);
}

void test_disk_read_write::decodeDevices()
{
    TestSystemInfoFileLocator::setProcDiskStatsName(":/procDisksBlockDevices");
    DiskValues values;

    values = ProcDiskStatDecoder::getReadWriteBytes("sda");
    QCOMPARE(values.totalReadBytes, 1*sectorSize);
    QCOMPARE(values.totalWriteBytes, 2*sectorSize);

    values = ProcDiskStatDecoder::getReadWriteBytes("sdb");
    QCOMPARE(values.totalReadBytes, 3*sectorSize);
    QCOMPARE(values.totalWriteBytes, 4*sectorSize);

    values = ProcDiskStatDecoder::getReadWriteBytes("mmcblk0");
    QCOMPARE(values.totalReadBytes, 5*sectorSize);
    QCOMPARE(values.totalWriteBytes, 6*sectorSize);

    values = ProcDiskStatDecoder::getReadWriteBytes("foo");
    QCOMPARE(values.totalReadBytes, 0);
    QCOMPARE(values.totalWriteBytes, 0);
}
