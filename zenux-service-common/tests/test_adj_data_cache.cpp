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
const char* cacheFileName = "test-cache";
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
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QVERIFY(QDir(testPath).exists());
}

void test_adj_data_cache::cachePathNotCreatedOnReadinvalidAdj()
{
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(!adjRW.readDataCached(cacheFileName));

    QVERIFY(!QDir(testPath).exists());
}

void test_adj_data_cache::cacheFileCreatedOnReadValidAdj()
{
    QVERIFY(MockEEprom24LC::mockReadFromFile(testI2cDevNode, testI2cAddr, ":/export_internal_initial_mt310s2.eeprom"));
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QVERIFY(QFile(adjRW.getCacheFullFileName(cacheFileName)).exists());
}

void test_adj_data_cache::cacheFileNotCreatedOnReadInvalidAdj()
{
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(!adjRW.readDataCached(cacheFileName));

    QVERIFY(!QFile(adjRW.getCacheFullFileName(cacheFileName)).exists());
}

void test_adj_data_cache::cacheFileCreatedSameContentAsEeprom()
{
    QVERIFY(MockEEprom24LC::mockReadFromFile(testI2cDevNode, testI2cAddr, ":/export_internal_initial_mt310s2.eeprom"));
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QByteArray expected = MockEEprom24LC::mockGetData(testI2cDevNode, testI2cAddr);

    QFile file(adjRW.getCacheFullFileName(cacheFileName));
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray written = file.readAll();

    QCOMPARE(expected, written);
}

void test_adj_data_cache::cacheFileCreatedSameContentAsEepromOnOverwrite()
{
    QDir dir;
    QVERIFY(dir.mkpath(testPath));
    QFile::copy(":/export_mt310s2_short.eeprom", AdjustmentEepromReadWrite::getCacheFullFileName(cacheFileName));

    QVERIFY(MockEEprom24LC::mockReadFromFile(testI2cDevNode, testI2cAddr, ":/export_internal_initial_mt310s2.eeprom"));
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QByteArray expected = MockEEprom24LC::mockGetData(testI2cDevNode, testI2cAddr);

    QFile file(adjRW.getCacheFullFileName(cacheFileName));
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray written = file.readAll();

    QCOMPARE(expected, written);
}

void test_adj_data_cache::cacheFileNotReadOnInvalidEeprom()
{
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(!adjRW.readDataCached(cacheFileName));

    QCOMPARE(MockEEprom24LC::mockGetReadCount(testI2cDevNode, testI2cAddr), 1);
}

void test_adj_data_cache::cacheFileIsEmpty()
{
    QDir dir;
    QVERIFY(dir.mkpath(testPath));
    QFile file(AdjustmentEepromReadWrite::getCacheFullFileName(cacheFileName));
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();

    QVERIFY(MockEEprom24LC::mockReadFromFile(testI2cDevNode, testI2cAddr, ":/export_internal_initial_mt310s2.eeprom"));
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QCOMPARE(MockEEprom24LC::mockGetReadCount(testI2cDevNode, testI2cAddr), 2);
}

void test_adj_data_cache::cacheFileIsTooShort()
{
    QDir dir;
    QVERIFY(dir.mkpath(testPath));
    QFile::copy(":/export_mt310s2_short.eeprom", AdjustmentEepromReadWrite::getCacheFullFileName(cacheFileName));

    QVERIFY(MockEEprom24LC::mockReadFromFile(testI2cDevNode, testI2cAddr, ":/export_internal_initial_mt310s2.eeprom"));
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QCOMPARE(MockEEprom24LC::mockGetReadCount(testI2cDevNode, testI2cAddr), 2);
}

void test_adj_data_cache::cacheFileBitFlipped()
{
    QDir dir;
    QVERIFY(dir.mkpath(testPath));
    QFile::copy(":/export_mt310s2_bit_flipped.eeprom", AdjustmentEepromReadWrite::getCacheFullFileName(cacheFileName));

    QVERIFY(MockEEprom24LC::mockReadFromFile(testI2cDevNode, testI2cAddr, ":/export_internal_initial_mt310s2.eeprom"));
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QCOMPARE(MockEEprom24LC::mockGetReadCount(testI2cDevNode, testI2cAddr), 2);
}

void test_adj_data_cache::cacheFileReadSuccessfully()
{
    QDir dir;
    QVERIFY(dir.mkpath(testPath));
    QFile::copy(":/export_internal_initial_mt310s2.eeprom", AdjustmentEepromReadWrite::getCacheFullFileName(cacheFileName));

    QVERIFY(MockEEprom24LC::mockReadFromFile(testI2cDevNode, testI2cAddr, ":/export_internal_initial_mt310s2.eeprom"));
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QCOMPARE(MockEEprom24LC::mockGetReadCount(testI2cDevNode, testI2cAddr), 1);

    QByteArray written = adjRW.getData();
    QFile file(":/export_internal_initial_mt310s2.eeprom");
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray expected = file.readAll();

    QCOMPARE(expected, written);
}

void test_adj_data_cache::nocachePathCreatedOnReadValidAdj()
{
    QVERIFY(MockEEprom24LC::mockReadFromFile(testI2cDevNode, testI2cAddr, ":/export_internal_initial_mt310s2.eeprom"));
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(adjRW.readData());

    QVERIFY(!QDir(testPath).exists());
}

void test_adj_data_cache::nocacheFileCreatedOnReadValidAdj()
{
    QVERIFY(MockEEprom24LC::mockReadFromFile(testI2cDevNode, testI2cAddr, ":/export_internal_initial_mt310s2.eeprom"));
    AdjustmentEepromReadWrite adjRW(testI2cDevNode, testI2cAddr, I2cMultiplexerFactory::createNullMuxer());
    QVERIFY(adjRW.readData());

    QVERIFY(!QFile(adjRW.getCacheFullFileName(cacheFileName)).exists());
}
