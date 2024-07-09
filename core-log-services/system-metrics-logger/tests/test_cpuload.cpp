#include "test_cpuload.h"
#include "cpuload.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_cpuload)

void test_cpuload::test_initial_zero()
{
    CpuLoad cpuLoadAllCores(0);
    TestSystemInfoFileLocator::setProcStatusFileName(":/procStatOneCpuAllZero");

    QVERIFY(cpuLoadAllCores.canGetValue());
    QCOMPARE(cpuLoadAllCores.getValue(), 0.0);
}

void test_cpuload::test_nice_load()
{
    CpuLoad cpuLoadAllCores(0);
    CpuLoad cpuLoadCoreOne(1);
    TestSystemInfoFileLocator::setProcStatusFileName(":/procStatOneCpuNiceLoad");

    QCOMPARE(cpuLoadAllCores.getValue(), 50.0);
    QCOMPARE(cpuLoadCoreOne.getValue(), 25.0);
}

void test_cpuload::test_user_load_multiple()
{
    CpuLoad cpuLoadAllCores(0);
    CpuLoad cpuLoadCoreOne(1);

    TestSystemInfoFileLocator::setProcStatusFileName(":/procStatOneCpuUserLoad");

    QVERIFY(cpuLoadAllCores.canGetValue());
    QVERIFY(cpuLoadCoreOne.canGetValue());

    TestSystemInfoFileLocator::setProcStatusFileName(":/procStatOneCpuUserLoad1");
    QCOMPARE(cpuLoadAllCores.getValue(), 50.0);
    QCOMPARE(cpuLoadCoreOne.getValue(), 25.0);

    TestSystemInfoFileLocator::setProcStatusFileName(":/procStatOneCpuUserLoad2");
    QCOMPARE(cpuLoadAllCores.getValue(), 25.0);
    QCOMPARE(cpuLoadCoreOne.getValue(), 100.0);
}
