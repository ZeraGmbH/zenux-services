#include "test_fpgainterrupts.h"
#include "fpgainterrupts.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_fpgainterrupts)

void test_fpgainterrupts::test_interrupts()
{
    FpgaInterrupts interrupts;
    TestSystemInfoFileLocator::setProcInterruptFile(":/procInterruptsMT310s2");

    QVERIFY(interrupts.canGetValue());
    QCOMPARE(interrupts.getValue(), 91988);
}

void test_fpgainterrupts::test_multipleTicks()
{
    FpgaInterrupts interrupts;
    TestSystemInfoFileLocator::setProcInterruptFile(":/procInterruptsMT310s2");

    QCOMPARE(interrupts.getValue(), 91988);
    QCOMPARE(interrupts.getValue(), 0);
    TestSystemInfoFileLocator::setProcInterruptFile(":/procInterruptsMT310s2-next");
    QCOMPARE(interrupts.getValue(), 2);
    QCOMPARE(interrupts.getValue(), 0);
}

void test_fpgainterrupts::test_invalidFile()
{
    FpgaInterrupts interrupts;
    TestSystemInfoFileLocator::setProcInterruptFile(":/foo");

    QVERIFY(interrupts.canGetValue());
    QCOMPARE(interrupts.getValue(), qQNaN());
}

void test_fpgainterrupts::test_noFpgaInInterrupts()
{
    FpgaInterrupts interrupts;
    TestSystemInfoFileLocator::setProcInterruptFile(":/procInterruptsDevMachine");

    QVERIFY(interrupts.canGetValue());
    QCOMPARE(interrupts.getValue(), qQNaN());
}
