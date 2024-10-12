#include "test_disk_io_calculator.h"
#include "diskiocalculator.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_disk_io_calculator)

void test_disk_io_calculator::zeroValuesInvalidDevice()
{
    DiskIoCalculator ioCalc("foo");
    TestSystemInfoFileLocator::setProcDiskStatsName(":/procDisksTest1");
    DiskValues values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.totalReadBytes, 0);
    QCOMPARE(values.totalWriteBytes, 0);
}

void test_disk_io_calculator::zeroValuesOnInvalidProcFile()
{
    DiskIoCalculator ioCalc("disk1");
    TestSystemInfoFileLocator::setProcDiskStatsName("foo");
    DiskValues values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.totalReadBytes, 0);
    QCOMPARE(values.totalWriteBytes, 0);
}

void test_disk_io_calculator::sequenceDisk1()
{
    DiskIoCalculator ioCalc("disk1");
    DiskValues values;

    TestSystemInfoFileLocator::setProcDiskStatsName(":/procDisksTest1");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.totalReadBytes, 1*sectorSizeAllDevices);
    QCOMPARE(values.totalWriteBytes, 2*sectorSizeAllDevices);

    TestSystemInfoFileLocator::setProcDiskStatsName(":/procDisksTest2");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.totalReadBytes, 1*sectorSizeAllDevices);
    QCOMPARE(values.totalWriteBytes, 1*sectorSizeAllDevices);

    TestSystemInfoFileLocator::setProcDiskStatsName(":/procDisksTest3");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.totalReadBytes, 2*sectorSizeAllDevices);
    QCOMPARE(values.totalWriteBytes, 2*sectorSizeAllDevices);
}

void test_disk_io_calculator::sequenceDisk2()
{
    DiskIoCalculator ioCalc("disk2");
    DiskValues values;

    TestSystemInfoFileLocator::setProcDiskStatsName(":/procDisksTest1");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.totalReadBytes, 2*sectorSizeAllDevices);
    QCOMPARE(values.totalWriteBytes, 4*sectorSizeAllDevices);

    TestSystemInfoFileLocator::setProcDiskStatsName(":/procDisksTest2");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.totalReadBytes, 2*sectorSizeAllDevices);
    QCOMPARE(values.totalWriteBytes, 4*sectorSizeAllDevices);

    TestSystemInfoFileLocator::setProcDiskStatsName(":/procDisksTest3");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.totalReadBytes, 0*sectorSizeAllDevices);
    QCOMPARE(values.totalWriteBytes, 0*sectorSizeAllDevices);
}
