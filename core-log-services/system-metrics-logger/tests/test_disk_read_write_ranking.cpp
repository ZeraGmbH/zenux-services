#include "test_disk_read_write_ranking.h"
#include "diskiototopranking.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_disk_read_write_ranking)

void test_disk_read_write_ranking::initTestCase()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_pid_io/default");
}

void test_disk_read_write_ranking::twoNonZeroIosCreateTwoEntries()
{
    DiskValuesProcesses processValues;
    processValues[1] = { 1, 2 };
    processValues[2] = { 2, 1 };

    DiskReadWriteRanking ranking = DiskIoToTopRanking::createRanking(processValues);
    QCOMPARE(ranking.readRanking.size(), 2);
    QCOMPARE(ranking.writeRanking.size(), 2);
}

void test_disk_read_write_ranking::zeroIosCreateNoEntries()
{
    DiskValuesProcesses processValues;
    processValues[1] = { 2, 2 };
    processValues[2] = { 1, 0 };
    processValues[3] = { 0, 1 };

    DiskReadWriteRanking ranking = DiskIoToTopRanking::createRanking(processValues);
    QCOMPARE(ranking.readRanking.size(), 2);
    QCOMPARE(ranking.writeRanking.size(), 2);
}

void test_disk_read_write_ranking::validProcessNames()
{
    DiskValuesProcesses processValues;
    processValues[1] = { 3, 3 };
    processValues[2] = { 2, 2 };
    processValues[3] = { 1, 1 };

    DiskReadWriteRanking ranking = DiskIoToTopRanking::createRanking(processValues);
    QCOMPARE(ranking.readRanking.size(), 3);
    QCOMPARE(ranking.readRanking[0].processName, "foo");
    QCOMPARE(ranking.readRanking[1].processName, "bar");
    QCOMPARE(ranking.readRanking[2].processName, "baz");
    QCOMPARE(ranking.writeRanking.size(), 3);
    QCOMPARE(ranking.writeRanking[0].processName, "foo");
    QCOMPARE(ranking.writeRanking[1].processName, "bar");
    QCOMPARE(ranking.writeRanking[2].processName, "baz");
}

void test_disk_read_write_ranking::goneProcessNamesDontCreateEntry()
{
    DiskValuesProcesses processValues;
    processValues[1] = { 2, 2 };
    processValues[2] = { 1, 1 };
    processValues[42] = { 1, 1 };

    DiskReadWriteRanking ranking = DiskIoToTopRanking::createRanking(processValues);
    QCOMPARE(ranking.readRanking.size(), 2);
    QCOMPARE(ranking.writeRanking.size(), 2);
}

void test_disk_read_write_ranking::validByteNums()
{
    DiskValuesProcesses processValues;
    processValues[1] = { 2, 4 };
    processValues[2] = { 1, 3 };

    DiskReadWriteRanking ranking = DiskIoToTopRanking::createRanking(processValues);
    QCOMPARE(ranking.readRanking[0].ioBytes, 2);
    QCOMPARE(ranking.readRanking[1].ioBytes, 1);
    QCOMPARE(ranking.writeRanking[0].ioBytes, 4);
    QCOMPARE(ranking.writeRanking[1].ioBytes, 3);
}

void test_disk_read_write_ranking::validTotalNums()
{
    DiskValuesProcesses processValues;
    processValues[1] = { 3, 6 };
    processValues[2] = { 1, 2 };

    DiskReadWriteRanking ranking = DiskIoToTopRanking::createRanking(processValues);
    QCOMPARE(ranking.totalBytesRead, 4);
    QCOMPARE(ranking.totalBytesWritten, 8);
}

void test_disk_read_write_ranking::goneProcessNotInTotalNums()
{
    DiskValuesProcesses processValues;
    processValues[1] = { 3, 6 };
    processValues[2] = { 1, 2 };
    processValues[42] = { 10, 100 };

    DiskReadWriteRanking ranking = DiskIoToTopRanking::createRanking(processValues);
    QCOMPARE(ranking.totalBytesRead, 4);
    QCOMPARE(ranking.totalBytesWritten, 8);
}

void test_disk_read_write_ranking::checkSort()
{
    DiskValuesProcesses processValues;
    processValues[1] = { 3, 6 };    // foo
    processValues[2] = { 10, 1 };   // bar
    processValues[3] = { 1, 200 };  // baz

    DiskReadWriteRanking ranking = DiskIoToTopRanking::createRanking(processValues);
    QCOMPARE(ranking.readRanking.size(), 3);
    QCOMPARE(ranking.readRanking[0].processName, "bar");
    QCOMPARE(ranking.readRanking[1].processName, "foo");
    QCOMPARE(ranking.readRanking[2].processName, "baz");
    QCOMPARE(ranking.writeRanking.size(), 3);
    QCOMPARE(ranking.writeRanking[0].processName, "baz");
    QCOMPARE(ranking.writeRanking[1].processName, "foo");
    QCOMPARE(ranking.writeRanking[2].processName, "bar");
}

void test_disk_read_write_ranking::checkSortIdenticalIos()
{
    DiskValuesProcesses processValues;
    processValues[1] = { 3, 6 };    // foo
    processValues[2] = { 3, 1 };    // bar
    processValues[3] = { 10, 6 };   // baz

    DiskReadWriteRanking ranking = DiskIoToTopRanking::createRanking(processValues);
    QCOMPARE(ranking.readRanking.size(), 3);
    QCOMPARE(ranking.readRanking[0].processName, "baz");
    QCOMPARE(ranking.readRanking[1].processName, "foo");
    QCOMPARE(ranking.readRanking[2].processName, "bar");
    QCOMPARE(ranking.writeRanking.size(), 3);
    QCOMPARE(ranking.writeRanking[0].processName, "foo");
    QCOMPARE(ranking.writeRanking[1].processName, "baz");
    QCOMPARE(ranking.writeRanking[2].processName, "bar");
}
