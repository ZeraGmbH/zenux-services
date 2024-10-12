#include "test_disk_io_total_calculator.h"
#include "diskiototalcalculator.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_disk_io_total_calculator)

void test_disk_io_total_calculator::zeroValuesOnNoDevices()
{
    DiskIoTotalCalculator ioCalc((QStringList()));
    DiskValues values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.totalReadBytes, 0);
    QCOMPARE(values.totalWriteBytes, 0);
}

void test_disk_io_total_calculator::sequenceDisk1Disk2()
{
    DiskIoTotalCalculator ioCalc(QStringList() << "disk1" << "disk2");
    DiskValues values;

    TestSystemInfoFileLocator::setProcDiskStatsName(":/procDisksTest1");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.totalReadBytes, 3*sectorSizeAllDevices);
    QCOMPARE(values.totalWriteBytes, 6*sectorSizeAllDevices);

    TestSystemInfoFileLocator::setProcDiskStatsName(":/procDisksTest2");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.totalReadBytes, 3*sectorSizeAllDevices);
    QCOMPARE(values.totalWriteBytes, 5*sectorSizeAllDevices);

    TestSystemInfoFileLocator::setProcDiskStatsName(":/procDisksTest3");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.totalReadBytes, 2*sectorSizeAllDevices);
    QCOMPARE(values.totalWriteBytes, 2*sectorSizeAllDevices);
}
