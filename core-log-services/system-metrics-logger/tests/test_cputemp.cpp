#include "test_cputemp.h"
#include "cputemp.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_cputemp)

void test_cputemp::test_temp()
{
    CpuTemp temp;
    TestSystemInfoFileLocator::setSysTempRootPath(":/");

    QCOMPARE(true, temp.canGetValue());
    QCOMPARE(50, temp.getValue());
}

void test_cputemp::test_invalidDirectory()
{
    CpuTemp temp;
    TestSystemInfoFileLocator::setSysTempRootPath(":/foo");

    QCOMPARE(false, temp.canGetValue());
    QCOMPARE(-273, temp.getValue());
}
