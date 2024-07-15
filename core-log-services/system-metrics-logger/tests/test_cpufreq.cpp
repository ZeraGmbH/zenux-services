#include "test_cpufreq.h"
#include "log-components/cpufreq.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_cpufreq)

void test_cpufreq::cleanup()
{
    TestSystemInfoFileLocator::resetSysFreqPaths();
}

void test_cpufreq::test_freq_valid()
{
    CpuFreq freq;
    TestSystemInfoFileLocator::setSysCpuPath(":/cpufreq");

    QCOMPARE(freq.canGetValue(), true);
    QCOMPARE(freq.getValue(), 750);
}

void test_cpufreq::test_freq_invalid_dir()
{
    CpuFreq freq;
    TestSystemInfoFileLocator::setSysCpuPath(":/foo");

    QCOMPARE(freq.canGetValue(), false);
}
