#include "test_auto_journal_logger.h"
#include "autojournallogger.h"
#include "testonofffilegenerator.h"
#include "timerfactoryqtfortest.h"
#include "timemachinefortest.h"
#include <QTest>

QTEST_MAIN(test_auto_journal_logger)

constexpr int loggerPerioudMs = 100;

void test_auto_journal_logger::init()
{
    TimerFactoryQtForTest::enableTest();
    TimeMachineForTest::reset();
    TestOnOffFileGenerator::deleteOnFile();
    TestOnOffFileGenerator::deleteLogFile();

    AutoJournalLogger logger(TestOnOffFileGenerator::getTestLogPath());
    QVERIFY(!QFile::exists(logger.getLogFilePath()));
}

void test_auto_journal_logger::manualLog()
{
    AutoJournalLogger logger(TestOnOffFileGenerator::getTestLogPath());
    QVERIFY(logger.saveLogFileNow());
    QVERIFY(QFile::exists(logger.getLogFilePath()));
}

void test_auto_journal_logger::periodicLog()
{
    AutoJournalLogger logger(TestOnOffFileGenerator::getTestLogPath());
    logger.startPeriodicLog(loggerPerioudMs);
    QVERIFY(QFile::exists(logger.getLogFilePath())); // first log on start

    TestOnOffFileGenerator::deleteLogFile();
    TimeMachineForTest::getInstance()->processTimers(loggerPerioudMs);
    QVERIFY(QFile::exists(logger.getLogFilePath()));

    TestOnOffFileGenerator::deleteLogFile();
    TimeMachineForTest::getInstance()->processTimers(loggerPerioudMs);
    QVERIFY(QFile::exists(logger.getLogFilePath()));
}

void test_auto_journal_logger::periodicLogAndStop()
{
    AutoJournalLogger logger(TestOnOffFileGenerator::getTestLogPath());
    logger.startPeriodicLog(loggerPerioudMs);

    TimeMachineForTest::getInstance()->processTimers(loggerPerioudMs);
    QVERIFY(QFile::exists(logger.getLogFilePath()));

    TestOnOffFileGenerator::deleteLogFile();
    logger.stopPeriodicLog();

    TimeMachineForTest::getInstance()->processTimers(loggerPerioudMs);
    QVERIFY(!QFile::exists(logger.getLogFilePath()));
}

