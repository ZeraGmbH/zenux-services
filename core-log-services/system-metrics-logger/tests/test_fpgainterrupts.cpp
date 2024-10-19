#include "test_fpgainterrupts.h"
#include "fpgainterrupts.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_fpgainterrupts)

void test_fpgainterrupts::test_interrupts()
{
    FpgaInterrupts interrupts;
    TestSystemInfoFileLocator::setProcBasePath(":/proc_interrupts/mt310s2");

    QVERIFY(interrupts.canGetValue());
    QCOMPARE(interrupts.getValue(), 91988);
}

void test_fpgainterrupts::test_multipleTicks()
{
    FpgaInterrupts interrupts;
    TestSystemInfoFileLocator::setProcBasePath(":/proc_interrupts/mt310s2");

    QCOMPARE(interrupts.getValue(), 91988);
    QCOMPARE(interrupts.getValue(), 0);
    TestSystemInfoFileLocator::setProcBasePath(":/proc_interrupts/mt310s2_next");
    QCOMPARE(interrupts.getValue(), 2);
    QCOMPARE(interrupts.getValue(), 0);
}

void test_fpgainterrupts::test_invalidFile()
{
    FpgaInterrupts interrupts;
    TestSystemInfoFileLocator::setProcBasePath(":/foo");

    QVERIFY(interrupts.canGetValue());
    QCOMPARE(interrupts.getValue(), qQNaN());
}

void test_fpgainterrupts::test_noFpgaInInterrupts()
{
    FpgaInterrupts interrupts;
    TestSystemInfoFileLocator::setProcBasePath(":/proc_interrupts/dev_machine");

    QVERIFY(interrupts.canGetValue());
    QCOMPARE(interrupts.getValue(), qQNaN());
}
