#include "test_fileutils.h"
#include "fileutils.h"
#include <QTest>

QTEST_MAIN(test_fileutils)

void test_fileutils::dirNotExist()
{
    QCOMPARE(FileUtils::dirExists("/foo/bar"), false);
}

void test_fileutils::dirExists()
{
    QCOMPARE(FileUtils::dirExists("/tmp/bar"), true);
    QCOMPARE(FileUtils::dirExists("/tmp/../bar"), true);
    QCOMPARE(FileUtils::dirExists("/tmp/../../bar"), true); // interpreted as '/'
}
