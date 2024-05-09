#include "test_adj_data_cache.h"
#include "adjustmenteepromreadwrite.h"
#include <QDir>
#include <QTest>

QTEST_MAIN(test_adj_data_cache)

static const char* testPath = "/tmp/adjTestCache";

void test_adj_data_cache::cleanup()
{
    QDir dir(testPath);
    dir.removeRecursively();
}

void test_adj_data_cache::pathCreatedOnFirstUse()
{
    AdjustmentEepromReadWrite::setCachePath(testPath);
    AdjustmentEepromReadWrite adjRW("foo", 0, nullptr);
    QVERIFY(QDir(testPath).exists());
}
