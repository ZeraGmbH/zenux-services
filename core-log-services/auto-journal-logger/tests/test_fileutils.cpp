#include "test_fileutils.h"
#include "fileutils.h"
#include <QTest>
#include <QDir>

QTEST_MAIN(test_fileutils)

void test_fileutils::dirNotExist()
{
    QCOMPARE(FileUtils::dirExistsForFileName("/foo/bar"), false);
}

void test_fileutils::dirExistsForFileName()
{
    QCOMPARE(FileUtils::dirExistsForFileName("/tmp/bar"), true);
    QCOMPARE(FileUtils::dirExistsForFileName("/tmp/../bar"), true);
    QCOMPARE(FileUtils::dirExistsForFileName("/tmp/../../bar"), true); // interpreted as '/'
}

void test_fileutils::dirFindExistingParent()
{
    QCOMPARE(FileUtils::getExistingParentNonRootPath("/tmp"), "/tmp");
    QCOMPARE(FileUtils::getExistingParentNonRootPath("/tmp/bar"), "/tmp");
    QCOMPARE(FileUtils::getExistingParentNonRootPath("/tmp/../tmp/bar"), "/tmp");
    QCOMPARE(FileUtils::getExistingParentNonRootPath("/tmp/foo/bar"), "/tmp");
    QCOMPARE(FileUtils::getExistingParentNonRootPath("foo/bar"), "");
    QCOMPARE(FileUtils::getExistingParentNonRootPath("/foo/bar"), "");
    QCOMPARE(FileUtils::getExistingParentNonRootPath("/tmp/../foo/bar"), "");
    QCOMPARE(FileUtils::getExistingParentNonRootPath("/tmp/foo/../bar"), "/tmp");

    QDir dir;
    dir.mkpath("/tmp/baz");
    QVERIFY(dir.exists("/tmp/baz"));
    QCOMPARE(FileUtils::getExistingParentNonRootPath("/tmp/baz"), "/tmp/baz");
    QCOMPARE(FileUtils::getExistingParentNonRootPath("/tmp/foo/../baz"), "/tmp/baz");
    dir.remove("/tmp/baz");
}

void test_fileutils::dirFindExistingParentOfFile()
{
    QFile file("/tmp/fooEmptyFile");
    QVERIFY(file.open(QFile::WriteOnly));
    file.close();
    QVERIFY(QFile::exists("/tmp/fooEmptyFile"));
    QCOMPARE(FileUtils::getExistingParentNonRootPath("/tmp/fooEmptyFile"), "/tmp");
    file.remove();
}
