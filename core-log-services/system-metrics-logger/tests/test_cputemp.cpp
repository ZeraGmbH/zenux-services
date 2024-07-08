#include "test_cputemp.h"
#include "cputemp.h"
#include "testsysteminfofilelocator.h"

#include "logcomponent.h"
#include "logstrategy.h"
#include "logvaluegetter.h"

#include <QTest>

QTEST_MAIN(test_cputemp)

void test_cputemp::test_temp()
{
    CpuTemp temp;
    TestSystemInfoFileLocator::setSysTempRootPath(":/");

    QCOMPARE(50, temp.getTemperature());
}

void test_cputemp::test_invalidDirectory()
{
    CpuTemp temp;
    TestSystemInfoFileLocator::setSysTempRootPath(":/foo");
    temp.getTemperature();

    QCOMPARE(-273, temp.getTemperature());
}

void test_cputemp::test_logComponent()
{
    TestSystemInfoFileLocator::setSysTempRootPath(":/");
    LogComponent temperatureComponent(std::make_unique<CpuTemp>(), std::make_unique<LogStrategy>());
    temperatureComponent.tryLogOne();
    QList<float> test(temperatureComponent.getBuffer());

    QCOMPARE(test[0], 50.0);
}

void test_cputemp::test_logComponentEmptyAfterTen()
{
    TestSystemInfoFileLocator::setSysTempRootPath(":/");
    LogComponent temperatureComponent(std::make_unique<CpuTemp>(), std::make_unique<LogStrategy>());
    for(int i = 0; i < 10; i++)
        temperatureComponent.tryLogOne();
    QList<float> test(temperatureComponent.getBuffer());

    QVERIFY(test.empty());
}

