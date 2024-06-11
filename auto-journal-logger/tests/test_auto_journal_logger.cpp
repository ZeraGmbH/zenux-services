#include "test_auto_journal_logger.h"
#include "autojournallogger.h"
#include "journallogcreator.h"
#include "testonofffilegenerator.h"
#include "timerfactoryqtfortest.h"
#include "timemachinefortest.h"
#include <QFile>
#include <QTest>

QTEST_MAIN(test_auto_journal_logger)

constexpr int loggerPerioudMs = 100;
static const char* fullLogFileName = "/tmp/test-zenux-log";

static AbstractLogCreatorPtr createLogCreator()
{
    return std::make_unique<JournalLogCreator>(fullLogFileName);
}

void test_auto_journal_logger::init()
{
    TimerFactoryQtForTest::enableTest();
    TimeMachineForTest::reset();
    TestOnOffFileGenerator::deleteOnFile();
    deleteLogFile();

    AutoJournalLogger logger(createLogCreator());
    QVERIFY(!QFile::exists(fullLogFileName));
}

void test_auto_journal_logger::manualLog()
{
    AutoJournalLogger logger(createLogCreator());
    QVERIFY(logger.saveLogFileNow());
    QVERIFY(QFile::exists(fullLogFileName));
}

void test_auto_journal_logger::periodicLog()
{
    AutoJournalLogger logger(createLogCreator());
    logger.startPeriodicLog(loggerPerioudMs);
    QVERIFY(QFile::exists(fullLogFileName)); // first log on start

    deleteLogFile();
    TimeMachineForTest::getInstance()->processTimers(loggerPerioudMs);
    QVERIFY(QFile::exists(fullLogFileName));

    deleteLogFile();
    TimeMachineForTest::getInstance()->processTimers(loggerPerioudMs);
    QVERIFY(QFile::exists(fullLogFileName));
}

void test_auto_journal_logger::periodicLogAndStop()
{
    AutoJournalLogger logger(createLogCreator());
    logger.startPeriodicLog(loggerPerioudMs);

    TimeMachineForTest::getInstance()->processTimers(loggerPerioudMs);
    QVERIFY(QFile::exists(fullLogFileName));

    deleteLogFile();
    logger.stopPeriodicLog();

    TimeMachineForTest::getInstance()->processTimers(loggerPerioudMs);
    QVERIFY(!QFile::exists(fullLogFileName));
}

void test_auto_journal_logger::deleteLogFile()
{
    QFile::remove(fullLogFileName);
}

