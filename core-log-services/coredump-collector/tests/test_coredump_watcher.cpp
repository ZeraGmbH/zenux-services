#include "test_coredump_watcher.h"
#include "coredumpwatcher.h"
#include <timemachineobject.h>
#include <QDir>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_coredump_watcher)

const static char* testCoreDumpDir = "/tmp/test_coredump_watcher";

void test_coredump_watcher::init()
{
    QDir dir;
    dir.mkpath(testCoreDumpDir);
}

void test_coredump_watcher::cleanup()
{
    QDir dir(testCoreDumpDir);
    dir.removeRecursively();
}

void test_coredump_watcher::test_initial_find_one()
{
    QFile file("core.vf-declarative-.10000.ignore.ignore.ignore.xz");
    QVERIFY(file.open(QFile::WriteOnly));
    QVERIFY(file.write("123"));
    file.close();

    CoreDumpWatcher watcher(testCoreDumpDir, QList<int>() << 10000 << 15000);
    QSignalSpy spy(&watcher, &CoreDumpWatcher::sigCoredumpCreated);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
}
