#include "test_proc_pid_details.h"
#include "procpiddetails.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_proc_pid_details)

void test_proc_pid_details::checkForRealSystemd()
{
    TestSystemInfoFileLocator::setProcBasePath("/proc");
    QCOMPARE(ProcPidDetails::getProcesName(1), "systemd");
}

void test_proc_pid_details::checkDifferentFormats()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_initial");
    QCOMPARE(ProcPidDetails::getProcesName(1), "foo");
    QCOMPARE(ProcPidDetails::getProcesName(2), "bar");
    QCOMPARE(ProcPidDetails::getProcesName(3), "baz");
}
