#include "test_logstrategyfuture.h"
#include "logstrategyfutureminmaxmean.h"
#include <QTest>

QTEST_MAIN(test_logstrategyfuture)

void test_logstrategyfuture::test_someNaNInput()
{
    QString logOutput;
    LogStrategyFutureMinMaxMean strat("foo", "bar", [&](QString log){
        logOutput = log;
    });
    QList<float> values;
    strat.addValue(values, qQNaN());
    strat.addValue(values, qQNaN());
    strat.addValue(values, qQNaN());
    strat.addValue(values, qQNaN());
    strat.addValue(values, 5);
    strat.addValue(values, 6);
    strat.addValue(values, 7);
    strat.addValue(values, 8);
    strat.addValue(values, 9);
    strat.addValue(values, 10);

    QCOMPARE(logOutput, "foo (bar) min: 5.0, max: 10.0, mean: 7.5");
    QVERIFY(values.isEmpty());
}

void test_logstrategyfuture::test_allNaNInput()
{
    QString logOutput = "foo";
    LogStrategyFutureMinMaxMean strat("foo", "bar", [&](QString log){
        logOutput = log;
    });
    QList<float> values;
    for(int i=0; i<10;i++)
        strat.addValue(values, qQNaN());

    QCOMPARE(logOutput, "foo");
    QVERIFY(values.isEmpty());
}

void test_logstrategyfuture::test_allZeroInput()
{
    QString logOutput = "foo";
    LogStrategyFutureMinMaxMean strat("foo", "bar", [&](QString log){
        logOutput = log;
    });
    QList<float> values;
    for(int i=0; i<10;i++)
        strat.addValue(values, 0.0);

    QCOMPARE(logOutput, "foo (bar) min: 0.0, max: 0.0, mean: 0.0");
    QVERIFY(values.isEmpty());

}
