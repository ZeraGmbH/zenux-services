#include "test_adj_data_cache.h"
#include "adjustmenteepromreadwrite.h"
#include "mocki2ceepromiofactory.h"
#include <i2cmultiplexerfactory.h>
#include "mockeeprom24lc.h"
#include <QDir>
#include <QFile>
#include <QTest>

QTEST_MAIN(test_adj_data_cache)

static const char* testPath = "/tmp/adjTestCache";
static const char* testI2cDevNode = "i2ctest";
static const quint8 testI2cAddr = 42;

void test_adj_data_cache::initTestCase()
{
    MockI2cEEpromIoFactory::enableMock();
    AdjustmentEepromReadWrite::setCachePath(testPath);
}

void test_adj_data_cache::init()
{
    MockEEprom24LC::mockCleanAll();
    QDir dir(testPath);
    dir.removeRecursively();
}

void test_adj_data_cache::cachePathCreatedOnReadValidAdj()
{
    QVERIFY(MockEEprom24LC::mockReadFromFile(testI2cDevNode, testI2cAddr, ":/export_internal_initial_mt310s2.eeprom"));
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(adjRW.readData());

    QVERIFY(QDir(testPath).exists());
}

void test_adj_data_cache::cachePathNotCreatedOnReadinvalidAdj()
{
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(!adjRW.readData());

    QVERIFY(!QDir(testPath).exists());
}

void test_adj_data_cache::cacheFileCreatedOnReadValidAdj()
{
    QVERIFY(MockEEprom24LC::mockReadFromFile(testI2cDevNode, testI2cAddr, ":/export_internal_initial_mt310s2.eeprom"));
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(adjRW.readData());

    QVERIFY(QFile(adjRW.getCacheFileName()).exists());
}

void test_adj_data_cache::cacheFileNotCreatedOnReadInvalidAdj()
{
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(!adjRW.readData());

    QVERIFY(!QFile(adjRW.getCacheFileName()).exists());
}

void test_adj_data_cache::cacheFileCreatedSameContentAsEEprom()
{
    QVERIFY(MockEEprom24LC::mockReadFromFile(testI2cDevNode, testI2cAddr, ":/export_internal_initial_mt310s2.eeprom"));
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(adjRW.readData());

    QByteArray expected = MockEEprom24LC::mockGetData(testI2cDevNode, testI2cAddr);

    QFile file(adjRW.getCacheFileName());
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray written = file.readAll();

    QCOMPARE(expected, written);
}
