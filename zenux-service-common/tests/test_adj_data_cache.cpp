#include "test_adj_data_cache.h"
#include "adjustmenteepromreadwrite.h"
#include <i2cmultiplexerfactory.h>
#include <QDir>
#include <QTest>

QTEST_MAIN(test_adj_data_cache)

static const char* testPath = "/tmp/adjTestCache";
static const char* testI2cDevNode = "i2ctest";
static const quint8 testI2cAddr = 42;

void test_adj_data_cache::initTestCase()
{
    AdjustmentEepromReadWrite::setCachePath(testPath);
}

void test_adj_data_cache::cleanup()
{
    QDir dir(testPath);
    dir.removeRecursively();
}

void test_adj_data_cache::pathCreatedOnFirstUse()
{
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(QDir(testPath).exists());
}
