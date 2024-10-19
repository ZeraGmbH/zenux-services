#include "test_disk_read_write.h"
#include "procdiskstatdecoder.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_disk_read_write)

void test_disk_read_write::allDiskBlockDevices()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_diskstats/default");
    QStringList devices = ProcDiskStatDecoder::getAllDiskBlockDevices();
    QCOMPARE(devices.count(), 17);
}

void test_disk_read_write::allDiskBlockDevicesInvalidProcFile()
{
    TestSystemInfoFileLocator::setProcBasePath("foo");
    QStringList devices = ProcDiskStatDecoder::getAllDiskBlockDevices();
    QCOMPARE(devices.count(), 0);
}

void test_disk_read_write::diskBlockDevicesOfInterest()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_diskstats/default");
    QStringList devices = ProcDiskStatDecoder::getDiskBlockDevicesOfInterest();
    QCOMPARE(devices.count(), 5);
    QVERIFY(devices.contains("sda"));
    QVERIFY(devices.contains("sdb"));
    QVERIFY(devices.contains("sdc"));
    QVERIFY(devices.contains("mmcblk0"));
    QVERIFY(devices.contains("mmcblk1"));
}

void test_disk_read_write::decodeLine()
{
    DiskValues values;
    values = ProcDiskStatDecoder::decodeSingleDiskStatLine("0      0 mmcblk1 0 0 1 0 0 0 2 0 0 0 0 0 0 0 0");
    QCOMPARE(values.bytesRead, 1*sectorSizeAllDevices);
    QCOMPARE(values.bytesWritten, 2*sectorSizeAllDevices);
    values = ProcDiskStatDecoder::decodeSingleDiskStatLine("0      0 mmcblk1 0 0 3 0 0 0 4 0 0 0 0 0 0 0 0");
    QCOMPARE(values.bytesRead, 3*sectorSizeAllDevices);
    QCOMPARE(values.bytesWritten, 4*sectorSizeAllDevices);
}

void test_disk_read_write::decodeInvalidLines()
{
    DiskValues values;
    values = ProcDiskStatDecoder::decodeSingleDiskStatLine("0      0 mmcblk1 0 0 a 0 0 0 b 0 0 0 0 0 0 0 0");
    QCOMPARE(values.bytesRead, 0);
    QCOMPARE(values.bytesWritten, 0);
    values = ProcDiskStatDecoder::decodeSingleDiskStatLine("");
    QCOMPARE(values.bytesRead, 0);
    QCOMPARE(values.bytesWritten, 0);
}

void test_disk_read_write::decodeInvalidProcFile()
{
    TestSystemInfoFileLocator::setProcBasePath("foo");
    DiskValues values;

    values = ProcDiskStatDecoder::getReadWriteBytes("sda");
    QCOMPARE(values.bytesRead, 0);
    QCOMPARE(values.bytesWritten, 0);
}

void test_disk_read_write::decodeDevices()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_diskstats/default");
    DiskValues values;

    values = ProcDiskStatDecoder::getReadWriteBytes("sda");
    QCOMPARE(values.bytesRead, 1*sectorSizeAllDevices);
    QCOMPARE(values.bytesWritten, 2*sectorSizeAllDevices);

    values = ProcDiskStatDecoder::getReadWriteBytes("sdb");
    QCOMPARE(values.bytesRead, 3*sectorSizeAllDevices);
    QCOMPARE(values.bytesWritten, 4*sectorSizeAllDevices);

    values = ProcDiskStatDecoder::getReadWriteBytes("mmcblk0");
    QCOMPARE(values.bytesRead, 5*sectorSizeAllDevices);
    QCOMPARE(values.bytesWritten, 6*sectorSizeAllDevices);

    values = ProcDiskStatDecoder::getReadWriteBytes("foo");
    QCOMPARE(values.bytesRead, 0);
    QCOMPARE(values.bytesWritten, 0);
}
