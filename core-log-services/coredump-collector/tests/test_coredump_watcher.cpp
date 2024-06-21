#include "test_coredump_watcher.h"
#include "coredumpwatcher.h"
#include <timemachineobject.h>
#include <QDir>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_coredump_watcher)

const static char* testCoreDumpSourceDir = "/tmp/test_coredump_watcher/input";
const static char* testCoreDumpDestDir = "/tmp/test_coredump_watcher/output";

void test_coredump_watcher::init()
{
    QDir dir;
    dir.mkpath(testCoreDumpSourceDir);
    dir.mkpath(testCoreDumpDestDir);
}

void test_coredump_watcher::cleanup()
{
    QDir dir(testCoreDumpSourceDir);
    dir.removeRecursively();
    QDir destDir(testCoreDumpDestDir);
    destDir.removeRecursively();
}

void test_coredump_watcher::test_single_core_file_ok()
{
    CoreDumpWatcher watcher(testCoreDumpSourceDir, testCoreDumpDestDir, QList<int>() << 10000 << 15000);
    watcher.startWatching();
    QSignalSpy spy(&watcher, &CoreDumpWatcher::sigCoredumpCreated);

    writeTestCoreDump(10000, "vf-declarative-");

    TimeMachineObject::feedEventLoop();
    QFile file(QString(testCoreDumpDestDir) + "/core.vf-declarative-.10000.ignore.ignore.ignore.xz");
    QFileDevice::Permissions pems  = QFileDevice::ReadOwner | QFileDevice::WriteOwner |
                                    QFileDevice::ReadGroup |
                                    QFileDevice::ReadOther | QFileDevice::WriteOther |
                                    QFileDevice::WriteUser | QFileDevice::ReadUser;
    QFileDevice::Permissions actual = file.permissions();
    QCOMPARE(actual, pems);

    QCOMPARE(spy.count(), 1);
}

void test_coredump_watcher::test_multiple_core_file_ok()
{
    CoreDumpWatcher watcher(testCoreDumpSourceDir, testCoreDumpDestDir, QList<int>() << 10000 << 15000);
    watcher.startWatching();
    QSignalSpy spy(&watcher, &CoreDumpWatcher::sigCoredumpCreated);

    writeTestCoreDump(10000, "vf-declarative-");
    writeTestCoreDump(10000, "vf-declarative2-");
    writeTestCoreDump(10000, "vf-declarative3-");

    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 3);
}




void test_coredump_watcher::test_dir_instead_of_dump_file()
{
    CoreDumpWatcher watcher(testCoreDumpSourceDir, testCoreDumpDestDir, QList<int>() << 10000 << 15000);
    watcher.startWatching();
    QSignalSpy spy(&watcher, &CoreDumpWatcher::sigCoredumpCreated);

    QDir testDir;
    testDir.mkpath(QString(testCoreDumpSourceDir) + "/testDir/");

    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 0);
}

void test_coredump_watcher::test_only_filtered_uid_copy()
{
    CoreDumpWatcher watcher(testCoreDumpSourceDir, testCoreDumpDestDir, QList<int>() << 10000 << 15000);
    watcher.startWatching();
    QSignalSpy spy(&watcher, &CoreDumpWatcher::sigCoredumpCreated);

    writeTestCoreDump(10000, "vf-declarative-");
    writeTestCoreDump(10001, "vf-declarative2-");
    writeTestCoreDump(15000, "vf-declarative3-");
    writeTestCoreDump(15001, "vf-declarative3-");

    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 2);
}

void test_coredump_watcher::test_misnamed_core_file()
{
    CoreDumpWatcher watcher(testCoreDumpSourceDir, testCoreDumpDestDir, QList<int>() << 10000 << 15000);
    watcher.startWatching();
    QSignalSpy spy(&watcher, &CoreDumpWatcher::sigCoredumpCreated);

    // introduce "." randomly into name
    writeTestCoreDump(10000, "vf-declar.ative-");
    // butcher the name completely
    QFile file(QString(testCoreDumpSourceDir) + "/foo");
    QVERIFY(file.open(QFile::WriteOnly));
    QVERIFY(file.write("123"));
    file.close();

    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 0);
}

bool test_coredump_watcher::writeTestCoreDump(int userId, QString programName)
{
    QFile file(QString(testCoreDumpSourceDir) + "/core." + programName + "." + QString::number(userId) + ".ignore.ignore.ignore.xz");
    if(!file.open(QFile::WriteOnly))
        return false;
    if(!file.write("123"))
        return false;
    file.close();
    return true;
}
