#include "test_periodic_log_generator.h"
#include "periodiclogger.h"
#include "logcreatorjournalsimple.h"
#include "testonofffilegenerator.h"
#include "timerfactoryqtfortest.h"
#include "timemachinefortest.h"
#include "testlogcreator.h"
#include <QFile>
#include <QTest>

QTEST_MAIN(test_periodic_log_generator)

constexpr int loggerPerioudMs = 100;
static const char* fullLogFileName = "/tmp/test-zenux-log";

static AbstractLogCreatorPtr createLogCreator()
{
    return std::make_unique<TestLogCreator>(fullLogFileName);
}

void test_periodic_log_generator::init()
{
    TimerFactoryQtForTest::enableTest();
    TimeMachineForTest::reset();
    TestOnOffFileGenerator::deleteOnFile();
    deleteLogFile();

    PeriodicLogger logger(createLogCreator());
    QVERIFY(!QFile::exists(fullLogFileName));
}

void test_periodic_log_generator::manualLog()
{
    PeriodicLogger logger(createLogCreator());
    QVERIFY(logger.saveLogFileNow());
    QVERIFY(QFile::exists(fullLogFileName));
}

void test_periodic_log_generator::periodicLog()
{
    PeriodicLogger logger(createLogCreator());
    logger.startPeriodicLog(loggerPerioudMs);
    QVERIFY(QFile::exists(fullLogFileName)); // first log on start

    deleteLogFile();
    TimeMachineForTest::getInstance()->processTimers(loggerPerioudMs);
    QVERIFY(QFile::exists(fullLogFileName));

    deleteLogFile();
    TimeMachineForTest::getInstance()->processTimers(loggerPerioudMs);
    QVERIFY(QFile::exists(fullLogFileName));
}

void test_periodic_log_generator::periodicLogAndStop()
{
    PeriodicLogger logger(createLogCreator());
    logger.startPeriodicLog(loggerPerioudMs);

    TimeMachineForTest::getInstance()->processTimers(loggerPerioudMs);
    QVERIFY(QFile::exists(fullLogFileName));

    deleteLogFile();
    logger.stopPeriodicLog();

    TimeMachineForTest::getInstance()->processTimers(loggerPerioudMs);
    QVERIFY(!QFile::exists(fullLogFileName));
}

void test_periodic_log_generator::deleteLogFile()
{
    QFile::remove(fullLogFileName);
}

