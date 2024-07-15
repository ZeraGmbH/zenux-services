#include "test_cputemp.h"
#include "cputemp.h"
#include "testsysteminfofilelocator.h"
#include "logcomponent.h"
#include "logstrategyminmaxmean.h"
#include <QTest>

QTEST_MAIN(test_cputemp)

void test_cputemp::test_temp()
{
    CpuTemp temp;
    TestSystemInfoFileLocator::setSysTempRootPath(":/");

    QCOMPARE(true, temp.canGetValue());
    QCOMPARE(50, temp.getValue());
}

void test_cputemp::test_invalidDirectory()
{
    CpuTemp temp;
    TestSystemInfoFileLocator::setSysTempRootPath(":/foo");

    QCOMPARE(false, temp.canGetValue());
    QCOMPARE(-273, temp.getValue());
}

void test_cputemp::test_logComponent()
{
    TestSystemInfoFileLocator::setSysTempRootPath(":/");
    LogComponent temperatureComponent(std::make_unique<CpuTemp>(), std::make_unique<LogStrategyMinMaxMean>("foo", "°C"));
    temperatureComponent.tryLogOne();
    QList<float> test(temperatureComponent.getBuffer());

    QCOMPARE(test[0], 50.0);
}

void test_cputemp::test_logComponentEmptyAfterTen()
{
    TestSystemInfoFileLocator::setSysTempRootPath(":/");
    LogComponent temperatureComponent(std::make_unique<CpuTemp>(), std::make_unique<LogStrategyMinMaxMean>("foo", "°C"));
    for(int i = 0; i < 10; i++)
        temperatureComponent.tryLogOne();
    QList<float> test(temperatureComponent.getBuffer());

    QVERIFY(test.empty());
}

