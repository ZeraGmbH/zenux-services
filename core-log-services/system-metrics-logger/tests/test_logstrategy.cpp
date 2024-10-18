#include "test_logstrategy.h"
#include "logstrategyminmaxmean.h"
#include <QTest>

QTEST_MAIN(test_logstrategy)

void test_logstrategy::test_someNaNInput()
{
    QString logOutput;
    LogStrategyMinMaxMean strat(4, "foo", "bar", [&](QString log){
        logOutput = log;
    });
    strat.addValue(qQNaN());
    strat.addValue(qQNaN());
    strat.addValue(qQNaN());
    strat.addValue(qQNaN());
    strat.addValue(1);
    strat.addValue(3);
    strat.addValue(1);
    strat.addValue(3);

    QCOMPARE(logOutput, "foo (bar) min: 1.0, max: 3.0, mean: 2.0");
}

void test_logstrategy::test_allNaNInput()
{
    QString logOutput = "foo";
    LogStrategyMinMaxMean strat(10, "foo", "bar", [&](QString log){
        logOutput = log;
    });
    QList<float> values;
    for(int i=0; i<10;i++)
        strat.addValue(qQNaN());

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
        strat.addValue(0.0);

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
    strat.addValue(0);
    strat.addValue(4);
    strat.addValue(1);
    strat.addValue(3);
    strat.addValue(1);
    strat.addValue(3);

    QCOMPARE(logOutput, "foo (bar) min: 0.0, max: 4.0, mean: 2.0");
    QVERIFY(values.isEmpty());

}
