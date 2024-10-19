#include "test_proc_pid_details.h"
#include "procpiddetails.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_proc_pid_details)

void test_proc_pid_details::checkDifferentFormats()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_pid_io/default");
    QCOMPARE(ProcPidDetails::getProcesName(1), "foo");
    QCOMPARE(ProcPidDetails::getProcesName(2), "bar");
    QCOMPARE(ProcPidDetails::getProcesName(3), "baz");
}
