#include "test_logstrategy.h"
#include "logstrategyminmaxmean.h"
#include <QTest>

QTEST_MAIN(test_logstrategy)

void test_logstrategy::test_someNaNInput()
{
    QString logOutput;
    LogStrategyMinMaxMean strat(10, "foo", "bar", [&](QString log){
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

void test_logstrategy::test_allNaNInput()
{
    QString logOutput = "foo";
    LogStrategyMinMaxMean strat(10, "foo", "bar", [&](QString log){
        logOutput = log;
    });
    QList<float> values;
    for(int i=0; i<10;i++)
        strat.addValue(values, qQNaN());

    QCOMPARE(logOutput, "foo");
    QVERIFY(values.isEmpty());
}

void test_logstrategy::test_allZeroInput()
{
    QString logOutput = "foo";
    LogStrategyMinMaxMean strat(10, "foo", "bar", [&](QString log){
        logOutput = log;
    });
    QList<float> values;
    for(int i=0; i<10;i++)
        strat.addValue(values, 0.0);

    QCOMPARE(logOutput, "foo (bar) min: 0.0, max: 0.0, mean: 0.0");
    QVERIFY(values.isEmpty());

}

void test_logstrategy::test_6Values()
{
    QString logOutput;
    LogStrategyMinMaxMean strat(6, "foo", "bar", [&](QString log){
        logOutput = log;
    });
    QList<float> values;
    strat.addValue(values, 0);
    strat.addValue(values, 4);
    strat.addValue(values, 1);
    strat.addValue(values, 3);
    strat.addValue(values, 1);
    strat.addValue(values, 3);

    QCOMPARE(logOutput, "foo (bar) min: 0.0, max: 4.0, mean: 2.0");
    QVERIFY(values.isEmpty());

}
