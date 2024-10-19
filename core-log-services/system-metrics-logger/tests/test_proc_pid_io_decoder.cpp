#include "test_proc_pid_io_decoder.h"
#include "procpidiodecoder.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_proc_pid_io_decoder)

void test_proc_pid_io_decoder::pidInvalid()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_pid_io/default");
    DiskValues readWrites = ProcPidIoDecoder::getReadWrites(0);
    QCOMPARE(readWrites.bytesRead, 0);
    QCOMPARE(readWrites.bytesWritten, 0);
}

void test_proc_pid_io_decoder::pidValid1()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_pid_io/default");
    DiskValues readWrites = ProcPidIoDecoder::getReadWrites(1);
    QCOMPARE(readWrites.bytesRead, 0);
    QCOMPARE(readWrites.bytesWritten, 20);
}

void test_proc_pid_io_decoder::pidValid1TrailingPathSeparator()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_pid_io/default/");
    DiskValues readWrites = ProcPidIoDecoder::getReadWrites(1);
    QCOMPARE(readWrites.bytesRead, 0);
    QCOMPARE(readWrites.bytesWritten, 20);
}

void test_proc_pid_io_decoder::pidValid2()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_pid_io/default");
    DiskValues readWrites = ProcPidIoDecoder::getReadWrites(2);
    QCOMPARE(readWrites.bytesRead, 20);
    QCOMPARE(readWrites.bytesWritten, 0);
}

void test_proc_pid_io_decoder::pidValid12_Invalid3()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_pid_io/default");
    QVector<int> pids = QVector<int>() << 1 << 2 << 3;
    DiskValuesProcesses readWrites = ProcPidIoDecoder::getPidGroupReadWrites(pids);
    QCOMPARE(readWrites.size(), 2);
    QVERIFY(readWrites.contains(1));
    QCOMPARE(readWrites[1].bytesRead, 0);
    QCOMPARE(readWrites[1].bytesWritten, 20);
    QVERIFY(readWrites.contains(2));
    QCOMPARE(readWrites[2].bytesRead, 20);
    QCOMPARE(readWrites[2].bytesWritten, 0);
}
