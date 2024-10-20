#include "test_cpuload.h"
#include "cpuload.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_cpuload)

void test_cpuload::test_initial_zero()
{
    CpuLoad cpuLoadAllCores(0);
    TestSystemInfoFileLocator::setProcBasePath(":/proc_stat/one_cpu_all_zero");

    QVERIFY(cpuLoadAllCores.canGetValue());
    QCOMPARE(cpuLoadAllCores.getValue(), 0.0);
}

void test_cpuload::test_nice_load()
{
    CpuLoad cpuLoadAllCores(0);
    CpuLoad cpuLoadCoreOne(1);
    TestSystemInfoFileLocator::setProcBasePath(":/proc_stat/one_cpu_nice_load");

    QCOMPARE(cpuLoadAllCores.getValue(), 50.0);
    QCOMPARE(cpuLoadCoreOne.getValue(), 25.0);
}

void test_cpuload::test_user_load_multiple()
{
    CpuLoad cpuLoadAllCores(0);
    CpuLoad cpuLoadCoreOne(1);

    TestSystemInfoFileLocator::setProcBasePath(":/proc_stat/one_cpu_user_load");

    QVERIFY(cpuLoadAllCores.canGetValue());
    QVERIFY(cpuLoadCoreOne.canGetValue());

    TestSystemInfoFileLocator::setProcBasePath(":/proc_stat/one_cpu_user_load1");
    QCOMPARE(cpuLoadAllCores.getValue(), 50.0);
    QCOMPARE(cpuLoadCoreOne.getValue(), 25.0);

    TestSystemInfoFileLocator::setProcBasePath(":/proc_stat/one_cpu_user_load2");
    QCOMPARE(cpuLoadAllCores.getValue(), 25.0);
    QCOMPARE(cpuLoadCoreOne.getValue(), 100.0);
}

void test_cpuload::test_coreIdx_out_of_range()
{
    CpuLoad cpuLoadCoreOutOfRange(99);
    TestSystemInfoFileLocator::setProcBasePath(":/proc_stat/one_cpu_all_zero");

    QCOMPARE(cpuLoadCoreOutOfRange.canGetValue(), false);
}
