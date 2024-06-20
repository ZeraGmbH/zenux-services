#include "test_cmd_handler.h"
#include <QFile>
#include <QDir>
#include <QTest>

QTEST_MAIN(test_cmd_handler)

const char* testDumpPath = "/tmp/test_cmd_handler";

void test_cmd_handler::init()
{
    QDir dir;
    dir.mkpath(testDumpPath);
}

void test_cmd_handler::cleanup()
{
    QDir dir(testDumpPath);
    dir.removeRecursively();
}

void test_cmd_handler::filterCoreDumpFilesByUid()
{
    QFile file;
}
