#include "test_dsp_interrupt_logger.h"
#include "dspinterruptlogger.h"
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_dsp_interrupt_logger)

constexpr int test_period_ms = 10;
constexpr int test_interrupt_count = 42;

void test_dsp_interrupt_logger::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
}

void test_dsp_interrupt_logger::noSignalsForNoValuesAdded()
{
    DspInterruptLogger logger(test_period_ms);
    QSignalSpy spy(&logger, &DspInterruptLogger::sigLogNewData);

    TimeMachineForTest::getInstance()->processTimers(test_period_ms);
    QCOMPARE(spy.count(), 0);
}

void test_dsp_interrupt_logger::oneValueAdded()
{
    DspInterruptLogger logger(test_period_ms);
    QSignalSpy spy(&logger, &DspInterruptLogger::sigLogNewData);

    logger.addInterruptCount(test_interrupt_count);

    TimeMachineForTest::getInstance()->processTimers(test_period_ms);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], test_interrupt_count);
    QCOMPARE(spy[0][1], test_interrupt_count);
    QCOMPARE(spy[0][2], test_interrupt_count);
}

void test_dsp_interrupt_logger::twoValuesAdded()
{
    DspInterruptLogger logger(test_period_ms);
    QSignalSpy spy(&logger, &DspInterruptLogger::sigLogNewData);

    logger.addInterruptCount(0);
    logger.addInterruptCount(test_interrupt_count);

    TimeMachineForTest::getInstance()->processTimers(test_period_ms);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], 0);
    QCOMPARE(spy[0][1], test_interrupt_count);
    QCOMPARE(spy[0][2], test_interrupt_count/2);
}

void test_dsp_interrupt_logger::twoValuesAddedTwoTimeSlots()
{
    DspInterruptLogger logger(test_period_ms);
    QSignalSpy spy(&logger, &DspInterruptLogger::sigLogNewData);

    logger.addInterruptCount(0);
    logger.addInterruptCount(test_interrupt_count);
    TimeMachineForTest::getInstance()->processTimers(test_period_ms);
    QCOMPARE(spy.count(), 1);

    logger.addInterruptCount(test_interrupt_count);
    TimeMachineForTest::getInstance()->processTimers(test_period_ms);

    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy[0][0], 0);
    QCOMPARE(spy[0][1], test_interrupt_count);
    QCOMPARE(spy[0][2], test_interrupt_count/2);

    QCOMPARE(spy[1][0], test_interrupt_count);
    QCOMPARE(spy[1][1], test_interrupt_count);
    QCOMPARE(spy[1][2], test_interrupt_count);
}
