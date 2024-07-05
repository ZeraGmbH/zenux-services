#include "test_cputemp.h"
#include "cputemp.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_cputemp)

void test_cputemp::test_temp()
{
    CpuTemp temp;
    TestSystemInfoFileLocator::setSysTempRootPath(":/");

    QCOMPARE(50, temp.getTemperature());
}

void test_cputemp::test_invalidDirectory()
{
    CpuTemp temp;
    TestSystemInfoFileLocator::setSysTempRootPath(":/foo");
    temp.getTemperature();

    QCOMPARE(-1, temp.getTemperature());
}
