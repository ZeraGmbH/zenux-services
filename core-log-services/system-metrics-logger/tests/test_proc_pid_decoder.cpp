#include "test_proc_pid_decoder.h"
#include "procpiddecoder.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_proc_pid_decoder)

void test_proc_pid_decoder::checkInvalidProcPath()
{
    TestSystemInfoFileLocator::setProcBasePath(":/foo");
    QVector<int> pids = ProcPidDecoder::getPids();
    QCOMPARE(pids.size(), 0);
}

void test_proc_pid_decoder::checkValidProcPath()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_1");
    QVector<int> pids = ProcPidDecoder::getPids();
    QCOMPARE(pids.size(), 3);
    QVERIFY(pids.contains(1));
    QVERIFY(pids.contains(2));
    QVERIFY(pids.contains(3));
}
