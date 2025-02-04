#include "test_auto_journal_logger.h"
#include "autojournalloggerfacade.h"
#include "testlogcreator.h"
#include <timemachinefortest.h>
#include <simplecmdioclient.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_auto_journal_logger)

static const char* testDumpPath = "/tmp/test_auto_journal_logger";
static const char* testIp = "127.0.0.1";
static constexpr int testPort = 5000;
constexpr int testTimeout = 2000;

void test_auto_journal_logger::init()
{
    QDir dir;
    dir.mkpath(testDumpPath);
}

void test_auto_journal_logger::cleanup()
{
    QDir dir(testDumpPath);
    dir.removeRecursively();
}

void test_auto_journal_logger::interfaceRegression()
{
    AutoJournalLoggerFacade logGen(std::make_unique<TestLogCreator>("journal.log"), testDumpPath, testPort);
    SimpleCmdIoClient client(testIp, testPort, testTimeout);
    std::shared_ptr<QString> response = std::make_shared<QString>();
    client.startCmdTransparent("help", response);

    TimeMachineForTest::getInstance()->processTimers(testTimeout);

    QString expected = TestLogHelpers::loadFile(":/help-response.txt");
    QVERIFY(!expected.isEmpty());
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, *response));
}

void test_auto_journal_logger::wrongParamCount()
{
    AutoJournalLoggerFacade logGen(std::make_unique<TestLogCreator>("journal.log"), testDumpPath, testPort);
    SimpleCmdIoClient client(testIp, testPort, testTimeout);

    std::shared_ptr<QString> response = std::make_shared<QString>();
    client.startCmdTransparent("SaveLogAndDumps", response);

    TimeMachineForTest::getInstance()->processTimers(testTimeout);
    QVERIFY(response->contains("Wrong number of parameters"));
}

void test_auto_journal_logger::destinationPathEmpty()
{
    AutoJournalLoggerFacade logGen(std::make_unique<TestLogCreator>("journal.log"), testDumpPath, testPort);
    SimpleCmdIoClient client(testIp, testPort, testTimeout);

    std::shared_ptr<QString> response = std::make_shared<QString>();
    client.startCmdTransparent("SaveLogAndDumps,,,", response);

    TimeMachineForTest::getInstance()->processTimers(testTimeout);
    QVERIFY(response->contains("DestDir cannot be empty"));
}

void test_auto_journal_logger::destinationParentDirNotValid()
{
    AutoJournalLoggerFacade logGen(std::make_unique<TestLogCreator>("journal.log"), testDumpPath, testPort);
    SimpleCmdIoClient client(testIp, testPort, testTimeout);

    std::shared_ptr<QString> response = std::make_shared<QString>();
    client.startCmdTransparent("SaveLogAndDumps,/foo/bar,,", response);

    TimeMachineForTest::getInstance()->processTimers(testTimeout);
    QVERIFY(response->contains("DestDir '/foo/bar' invalid"));
}

void test_auto_journal_logger::versionFileNotValid()
{
    AutoJournalLoggerFacade logGen(std::make_unique<TestLogCreator>("journal.log"), testDumpPath, testPort);
    SimpleCmdIoClient client(testIp, testPort, testTimeout);

    std::shared_ptr<QString> response = std::make_shared<QString>();
    client.startCmdTransparent(QString("SaveLogAndDumps,%1/subdir,foo,").arg(testDumpPath), response);

    TimeMachineForTest::getInstance()->processTimers(testTimeout);
    QVERIFY(response->contains("VersionInfoFilePath: File 'foo' does not exist!"));
}

void test_auto_journal_logger::versionFileValid()
{
    const QString versionFileName = QString("%1/version-file").arg(testDumpPath);
    QFile versionFile(versionFileName);
    versionFile.open(QFile::WriteOnly);
    versionFile.write("123");
    versionFile.close();

    AutoJournalLoggerFacade logGen(std::make_unique<TestLogCreator>("journal.log"), testDumpPath, testPort);
    SimpleCmdIoClient client(testIp, testPort, testTimeout);

    std::shared_ptr<QString> response = std::make_shared<QString>();
    client.startCmdTransparent(QString("SaveLogAndDumps,%1/subdir,%2,").arg(testDumpPath, versionFileName), response);

    TimeMachineForTest::getInstance()->processTimers(testTimeout);

    QStringList foundFiles = searchFile(QString("%1/subdir").arg(testDumpPath), "test-version");
    QCOMPARE(foundFiles.count(), 1);

    /*QString expected = TestLogHelpers::loadFile(":/test-version");
    QVERIFY(!expected.isEmpty());
    QString stored = TestLogHelpers::loadFile(QString("%1/subdir/test-version").arg(testDumpPath));
    QVERIFY(!stored.isEmpty());
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, *response));
    QVERIFY(response->contains("OK"));*/
}

QStringList test_auto_journal_logger::searchFile(const QString &baseDirectory, const QString &fileName)
{
    QStringList foundFiles;

    // local dir
    QDir dir(baseDirectory);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList fileList = dir.entryList();
    for(const QString &entry : dir.entryList()) {
        if(entry == fileName)
            foundFiles.append(baseDirectory + "/" + entry);
    }

    // subdirs
    dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    for(const QString &entry : dir.entryList()) {
        QString subdir = QString("%1/%2").arg(dir.absolutePath(), entry);
        foundFiles.append(searchFile(subdir, fileName));
    }

    return foundFiles;
}
