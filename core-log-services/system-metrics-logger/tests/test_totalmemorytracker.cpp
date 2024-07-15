#include "test_totalmemorytracker.h"
#include "log-components/totalmemorytracker.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_totalmemorytracker)

void test_totalmemorytracker::test_invalid_values()
{
    TotalMemoryTracker memoryTracker;
    TestSystemInfoFileLocator::setProcMeminfoFileName(":/procMeminfAllZero");
    QCOMPARE(memoryTracker.canGetValue(), false);
}

void test_totalmemorytracker::test_memory_used()
{
    TotalMemoryTracker memoryTracker;
    TestSystemInfoFileLocator::setProcMeminfoFileName(":/procMeminfo");
    QCOMPARE(memoryTracker.canGetValue(), true);
    QCOMPARE((int)memoryTracker.getValue(), 27);
}
