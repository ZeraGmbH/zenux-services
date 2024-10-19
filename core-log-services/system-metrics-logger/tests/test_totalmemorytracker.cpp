#include "test_totalmemorytracker.h"
#include "totalmemorytracker.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_totalmemorytracker)

void test_totalmemorytracker::test_invalid_values()
{
    TotalMemoryTracker memoryTracker;
    TestSystemInfoFileLocator::setProcBasePath(":/proc_meminfo/all_zero");
    QCOMPARE(memoryTracker.canGetValue(), false);
}

void test_totalmemorytracker::test_memory_used()
{
    TotalMemoryTracker memoryTracker;
    TestSystemInfoFileLocator::setProcBasePath(":/proc_meminfo/default");
    QCOMPARE(memoryTracker.canGetValue(), true);
    QCOMPARE((int)memoryTracker.getValue(), 27);
}
