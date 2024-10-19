#include "test_disk_io_for_all_processes.h"
#include "diskioforallprocesses.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_disk_io_for_all_processes)

void test_disk_io_for_all_processes::canRead()
{
    DiskIoForAllProcesses dio;
    QVERIFY(dio.canGetValue());
}

void test_disk_io_for_all_processes::firstReadNoValues()
{
    DiskIoForAllProcesses dio;
    TestSystemInfoFileLocator::setProcBasePath(":/proc_initial");
    DiskValuesProcesses values = dio.getValue();
    QCOMPARE(values.size(), 0);
}

void test_disk_io_for_all_processes::secondReadDiffValues()
{
    DiskIoForAllProcesses dio;
    TestSystemInfoFileLocator::setProcBasePath(":/proc_initial");
    dio.getValue();

    TestSystemInfoFileLocator::setProcBasePath(":/proc_up");
    DiskValuesProcesses values = dio.getValue();
    QCOMPARE(values.size(), 2);
    QVERIFY(values.contains(1));
    QCOMPARE(values[1].bytesRead, 20);
    QCOMPARE(values[1].bytesWritten, 20);
    QVERIFY(values.contains(2));
    QCOMPARE(values[2].bytesRead, 20);
    QCOMPARE(values[2].bytesWritten, 60);
}

void test_disk_io_for_all_processes::thirdReadValuesDown()
{
    DiskIoForAllProcesses dio;
    TestSystemInfoFileLocator::setProcBasePath(":/proc_initial");
    dio.getValue();

    TestSystemInfoFileLocator::setProcBasePath(":/proc_up");
    dio.getValue();

    TestSystemInfoFileLocator::setProcBasePath(":/proc_down");
    DiskValuesProcesses values = dio.getValue();
    QCOMPARE(values.size(), 2);
    QVERIFY(values.contains(1));
    QCOMPARE(values[1].bytesRead, 0);
    QCOMPARE(values[1].bytesWritten, 0);
    QVERIFY(values.contains(2));
    QCOMPARE(values[2].bytesRead, 0);
    QCOMPARE(values[2].bytesWritten, 0);
}

void test_disk_io_for_all_processes::secondRemovesPid1ReadAgain()
{
    DiskIoForAllProcesses dio;
    TestSystemInfoFileLocator::setProcBasePath(":/proc_initial");
    dio.getValue();

    TestSystemInfoFileLocator::setProcBasePath(":/proc_pid1_missing");
    DiskValuesProcesses values = dio.getValue();
    QCOMPARE(values.size(), 1);
    QVERIFY(values.contains(2));
    QCOMPARE(values[2].bytesRead, 1);
    QCOMPARE(values[2].bytesWritten, 1);

    values = dio.getValue();
    QCOMPARE(values.size(), 1);
    QVERIFY(values.contains(2));
    QCOMPARE(values[2].bytesRead, 0);
    QCOMPARE(values[2].bytesWritten, 0);
}

void test_disk_io_for_all_processes::secondAddsPid1ReadAgain()
{
    DiskIoForAllProcesses dio;
    TestSystemInfoFileLocator::setProcBasePath(":/proc_pid1_missing");
    dio.getValue();

    TestSystemInfoFileLocator::setProcBasePath(":/proc_up");
    DiskValuesProcesses values = dio.getValue();
    QCOMPARE(values.size(), 1);
    QVERIFY(values.contains(2));
    QCOMPARE(values[2].bytesRead, 19);
    QCOMPARE(values[2].bytesWritten, 59);

    values = dio.getValue();
    QCOMPARE(values.size(), 2);
    QVERIFY(values.contains(1));
    QCOMPARE(values[1].bytesRead, 0);
    QCOMPARE(values[1].bytesWritten, 0);
    QVERIFY(values.contains(2));
    QCOMPARE(values[2].bytesRead, 0);
    QCOMPARE(values[2].bytesWritten, 0);
}
