#include "test_disk_io_calculator.h"
#include "diskiocalculator.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_disk_io_calculator)

void test_disk_io_calculator::zeroValuesInvalidDevice()
{
    DiskIoCalculator ioCalc("foo");
    TestSystemInfoFileLocator::setProcBasePath(":/proc_diskstats/test1");
    DiskValues values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.bytesRead, 0);
    QCOMPARE(values.bytesWritten, 0);
}

void test_disk_io_calculator::zeroValuesOnInvalidProcFile()
{
    DiskIoCalculator ioCalc("disk1");
    TestSystemInfoFileLocator::setProcBasePath("foo");
    DiskValues values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.bytesRead, 0);
    QCOMPARE(values.bytesWritten, 0);
}

void test_disk_io_calculator::sequenceDisk1()
{
    DiskIoCalculator ioCalc("disk1");
    DiskValues values;

    TestSystemInfoFileLocator::setProcBasePath(":/proc_diskstats/test1");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.bytesRead, 1*sectorSizeAllDevices);
    QCOMPARE(values.bytesWritten, 2*sectorSizeAllDevices);

    TestSystemInfoFileLocator::setProcBasePath(":/proc_diskstats/test2");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.bytesRead, 1*sectorSizeAllDevices);
    QCOMPARE(values.bytesWritten, 1*sectorSizeAllDevices);

    TestSystemInfoFileLocator::setProcBasePath(":/proc_diskstats/test3");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.bytesRead, 2*sectorSizeAllDevices);
    QCOMPARE(values.bytesWritten, 2*sectorSizeAllDevices);
}

void test_disk_io_calculator::sequenceDisk2()
{
    DiskIoCalculator ioCalc("disk2");
    DiskValues values;

    TestSystemInfoFileLocator::setProcBasePath(":/proc_diskstats/test1");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.bytesRead, 2*sectorSizeAllDevices);
    QCOMPARE(values.bytesWritten, 4*sectorSizeAllDevices);

    TestSystemInfoFileLocator::setProcBasePath(":/proc_diskstats/test2");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.bytesRead, 2*sectorSizeAllDevices);
    QCOMPARE(values.bytesWritten, 4*sectorSizeAllDevices);

    TestSystemInfoFileLocator::setProcBasePath(":/proc_diskstats/test3");
    values = ioCalc.getReadWriteSinceLast();
    QCOMPARE(values.bytesRead, 0*sectorSizeAllDevices);
    QCOMPARE(values.bytesWritten, 0*sectorSizeAllDevices);
}
