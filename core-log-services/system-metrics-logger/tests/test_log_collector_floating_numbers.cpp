#include "test_log_collector_floating_numbers.h"
#include "logcollectorfloatingnumbers.h"
#include <QTest>

QTEST_MAIN(test_log_collector_floating_numbers)

void test_log_collector_floating_numbers::noReturnOnOneBelow()
{
    LogCollectorFloatingNumbers<float> collector(3);
    QCOMPARE(collector.addValueAndReturnIfFull(1.0).size(), 0);
    QCOMPARE(collector.addValueAndReturnIfFull(2.0).size(), 0);
}

void test_log_collector_floating_numbers::returnFirstFull()
{
    LogCollectorFloatingNumbers<float> collector(3);
    QCOMPARE(collector.addValueAndReturnIfFull(1.0).size(), 0);
    QCOMPARE(collector.addValueAndReturnIfFull(2.0).size(), 0);
    QList<float> ret = collector.addValueAndReturnIfFull(3.0);
    QCOMPARE(ret.size(), 3);
    QCOMPARE(ret[0], 1.0);
    QCOMPARE(ret[1], 2.0);
    QCOMPARE(ret[2], 3.0);
}

void test_log_collector_floating_numbers::noReturnOnAfterFirstFull()
{
    LogCollectorFloatingNumbers<float> collector(3);
    QCOMPARE(collector.addValueAndReturnIfFull(1.0).size(), 0);
    QCOMPARE(collector.addValueAndReturnIfFull(2.0).size(), 0);
    QCOMPARE(collector.addValueAndReturnIfFull(3.0).size(), 3);
    QCOMPARE(collector.addValueAndReturnIfFull(4.0).size(), 0);
}

void test_log_collector_floating_numbers::returnSecondFull()
{
    LogCollectorFloatingNumbers<float> collector(3);
    QCOMPARE(collector.addValueAndReturnIfFull(1.0).size(), 0);
    QCOMPARE(collector.addValueAndReturnIfFull(2.0).size(), 0);
    QCOMPARE(collector.addValueAndReturnIfFull(3.0).size(), 3);
    QCOMPARE(collector.addValueAndReturnIfFull(4.0).size(), 0);
    QCOMPARE(collector.addValueAndReturnIfFull(5.0).size(), 0);
    QList<float> ret = collector.addValueAndReturnIfFull(6.0);
    QCOMPARE(ret.size(), 3);
    QCOMPARE(ret[0], 4.0);
    QCOMPARE(ret[1], 5.0);
    QCOMPARE(ret[2], 6.0);
}

void test_log_collector_floating_numbers::ignoreNan()
{
    LogCollectorFloatingNumbers<float> collector(3);
    QCOMPARE(collector.addValueAndReturnIfFull(qQNaN()).size(), 0);
    QCOMPARE(collector.addValueAndReturnIfFull(qQNaN()).size(), 0);
    QCOMPARE(collector.addValueAndReturnIfFull(1.0).size(), 0);
    QCOMPARE(collector.addValueAndReturnIfFull(2.0).size(), 0);
    QList<float> ret = collector.addValueAndReturnIfFull(3.0);
    QCOMPARE(ret.size(), 3);
    QCOMPARE(ret[0], 1.0);
    QCOMPARE(ret[1], 2.0);
    QCOMPARE(ret[2], 3.0);
}

void test_log_collector_floating_numbers::ignoreInf()
{
    LogCollectorFloatingNumbers<float> collector(3);
    QCOMPARE(collector.addValueAndReturnIfFull(qInf()).size(), 0);
    QCOMPARE(collector.addValueAndReturnIfFull(qInf()).size(), 0);
    QCOMPARE(collector.addValueAndReturnIfFull(1.0).size(), 0);
    QCOMPARE(collector.addValueAndReturnIfFull(2.0).size(), 0);
    QList<float> ret = collector.addValueAndReturnIfFull(3.0);
    QCOMPARE(ret.size(), 3);
    QCOMPARE(ret[0], 1.0);
    QCOMPARE(ret[1], 2.0);
    QCOMPARE(ret[2], 3.0);
}

void test_log_collector_floating_numbers::returnFirstFullDouble()
{
    LogCollectorFloatingNumbers<double> collector(3);
    QCOMPARE(collector.addValueAndReturnIfFull(1.0).size(), 0);
    QCOMPARE(collector.addValueAndReturnIfFull(2.0).size(), 0);
    QList<double> ret = collector.addValueAndReturnIfFull(3.0);
    QCOMPARE(ret.size(), 3);
    QCOMPARE(ret[0], 1.0);
    QCOMPARE(ret[1], 2.0);
    QCOMPARE(ret[2], 3.0);
}
