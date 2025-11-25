#include "test_adj_data_cache.h"
#include "adjustmenteepromreadwrite.h"
#include "mockeepromdevice.h"
#include <i2cmultiplexerfactory.h>
#include <testloghelpers.h>
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
    AdjustmentEepromReadWrite::setCachePath(testPath);
}

void test_adj_data_cache::init()
{
    MockEepromDevice::cleanAll();
    QDir dir(testPath);
    dir.removeRecursively();
}

void test_adj_data_cache::cachePathCreatedOnReadValidAdj()
{
    MockEepromDevice::setData({testI2cDevNode, testI2cAddr},
                              TestLogHelpers::loadFile(":/export_internal_initial_mt310s2.eeprom"));
    MockEepromI2cFactory eepromFactory;
    EepromI2cDeviceInterfacePtr adjMemory = eepromFactory.createEeprom({testI2cDevNode, testI2cAddr},
                                                                       AbstractEepromI2cDevice::capacity24LC256);
    AdjustmentEepromReadWrite adjRW(std::move(adjMemory));
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QVERIFY(QDir(testPath).exists());
}

void test_adj_data_cache::cachePathNotCreatedOnReadinvalidAdj()
{
    MockEepromI2cFactory eepromFactory;
    EepromI2cDeviceInterfacePtr adjMemory = eepromFactory.createEeprom({testI2cDevNode, testI2cAddr},
                                                                       AbstractEepromI2cDevice::capacity24LC256);
    AdjustmentEepromReadWrite adjRW(std::move(adjMemory));
    QVERIFY(!adjRW.readDataCached(cacheFileName));

    QVERIFY(!QDir(testPath).exists());
}

void test_adj_data_cache::cacheFileCreatedOnReadValidAdj()
{
    MockEepromDevice::setData({testI2cDevNode, testI2cAddr},
                              TestLogHelpers::loadFile(":/export_internal_initial_mt310s2.eeprom"));
    MockEepromI2cFactory eepromFactory;
    EepromI2cDeviceInterfacePtr adjMemory = eepromFactory.createEeprom({testI2cDevNode, testI2cAddr},
                                                                       AbstractEepromI2cDevice::capacity24LC256);
    AdjustmentEepromReadWrite adjRW(std::move(adjMemory));
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QVERIFY(QFile(adjRW.getCacheFullFileName(cacheFileName)).exists());
}

void test_adj_data_cache::cacheFileNotCreatedOnReadInvalidAdj()
{
    MockEepromI2cFactory eepromFactory;
    EepromI2cDeviceInterfacePtr adjMemory = eepromFactory.createEeprom({testI2cDevNode, testI2cAddr},
                                                                       AbstractEepromI2cDevice::capacity24LC256);
    AdjustmentEepromReadWrite adjRW(std::move(adjMemory));
    QVERIFY(!adjRW.readDataCached(cacheFileName));

    QVERIFY(!QFile(adjRW.getCacheFullFileName(cacheFileName)).exists());
}

void test_adj_data_cache::cacheFileCreatedSameContentAsEeprom()
{
    MockEepromDevice::setData({testI2cDevNode, testI2cAddr},
                              TestLogHelpers::loadFile(":/export_internal_initial_mt310s2.eeprom"));
    MockEepromI2cFactory eepromFactory;
    EepromI2cDeviceInterfacePtr adjMemory = eepromFactory.createEeprom({testI2cDevNode, testI2cAddr},
                                                                       AbstractEepromI2cDevice::capacity24LC256);
    AdjustmentEepromReadWrite adjRW(std::move(adjMemory));
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QByteArray expected = MockEepromDevice::getData({testI2cDevNode, testI2cAddr});

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

    MockEepromDevice::setData({testI2cDevNode, testI2cAddr},
                              TestLogHelpers::loadFile(":/export_internal_initial_mt310s2.eeprom"));
    MockEepromI2cFactory eepromFactory;
    EepromI2cDeviceInterfacePtr adjMemory = eepromFactory.createEeprom({testI2cDevNode, testI2cAddr},
                                                                       AbstractEepromI2cDevice::capacity24LC256);
    AdjustmentEepromReadWrite adjRW(std::move(adjMemory));
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QByteArray expected = MockEepromDevice::getData({testI2cDevNode, testI2cAddr});

    QFile file(adjRW.getCacheFullFileName(cacheFileName));
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray written = file.readAll();

    QCOMPARE(expected, written);
}

void test_adj_data_cache::cacheFileNotReadOnInvalidEeprom()
{
    MockEepromI2cFactory eepromFactory;
    EepromI2cDeviceInterfacePtr adjMemory = eepromFactory.createEeprom({testI2cDevNode, testI2cAddr},
                                                                       AbstractEepromI2cDevice::capacity24LC256);
    AdjustmentEepromReadWrite adjRW(std::move(adjMemory));
    QVERIFY(!adjRW.readDataCached(cacheFileName));

    QCOMPARE(MockEepromDevice::getReadCount({testI2cDevNode, testI2cAddr}), 1);
}

void test_adj_data_cache::cacheFileIsEmpty()
{
    QDir dir;
    QVERIFY(dir.mkpath(testPath));
    QFile file(AdjustmentEepromReadWrite::getCacheFullFileName(cacheFileName));
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();

    MockEepromDevice::setData({testI2cDevNode, testI2cAddr},
                              TestLogHelpers::loadFile(":/export_internal_initial_mt310s2.eeprom"));
    MockEepromI2cFactory eepromFactory;
    EepromI2cDeviceInterfacePtr adjMemory = eepromFactory.createEeprom({testI2cDevNode, testI2cAddr},
                                                                       AbstractEepromI2cDevice::capacity24LC256);
    AdjustmentEepromReadWrite adjRW(std::move(adjMemory));
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QCOMPARE(MockEepromDevice::getReadCount({testI2cDevNode, testI2cAddr}), 2);
}

void test_adj_data_cache::cacheFileIsTooShort()
{
    QDir dir;
    QVERIFY(dir.mkpath(testPath));
    QFile::copy(":/export_mt310s2_short.eeprom", AdjustmentEepromReadWrite::getCacheFullFileName(cacheFileName));

    MockEepromDevice::setData({testI2cDevNode, testI2cAddr},
                              TestLogHelpers::loadFile(":/export_internal_initial_mt310s2.eeprom"));
    MockEepromI2cFactory eepromFactory;
    EepromI2cDeviceInterfacePtr adjMemory = eepromFactory.createEeprom({testI2cDevNode, testI2cAddr},
                                                                       AbstractEepromI2cDevice::capacity24LC256);
    AdjustmentEepromReadWrite adjRW(std::move(adjMemory));
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QCOMPARE(MockEepromDevice::getReadCount({testI2cDevNode, testI2cAddr}), 2);
}

void test_adj_data_cache::cacheFileBitFlipped()
{
    QDir dir;
    QVERIFY(dir.mkpath(testPath));
    QFile::copy(":/export_mt310s2_bit_flipped.eeprom", AdjustmentEepromReadWrite::getCacheFullFileName(cacheFileName));

    MockEepromDevice::setData({testI2cDevNode, testI2cAddr},
                              TestLogHelpers::loadFile(":/export_internal_initial_mt310s2.eeprom"));
    MockEepromI2cFactory eepromFactory;
    EepromI2cDeviceInterfacePtr adjMemory = eepromFactory.createEeprom({testI2cDevNode, testI2cAddr},
                                                                       AbstractEepromI2cDevice::capacity24LC256);
    AdjustmentEepromReadWrite adjRW(std::move(adjMemory));
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QCOMPARE(MockEepromDevice::getReadCount({testI2cDevNode, testI2cAddr}), 2);
}

void test_adj_data_cache::cacheFileReadSuccessfully()
{
    QDir dir;
    QVERIFY(dir.mkpath(testPath));
    QFile::copy(":/export_internal_initial_mt310s2.eeprom", AdjustmentEepromReadWrite::getCacheFullFileName(cacheFileName));

    MockEepromDevice::setData({testI2cDevNode, testI2cAddr},
                              TestLogHelpers::loadFile(":/export_internal_initial_mt310s2.eeprom"));
    MockEepromI2cFactory eepromFactory;
    EepromI2cDeviceInterfacePtr adjMemory = eepromFactory.createEeprom({testI2cDevNode, testI2cAddr},
                                                                       AbstractEepromI2cDevice::capacity24LC256);
    AdjustmentEepromReadWrite adjRW(std::move(adjMemory));
    QVERIFY(adjRW.readDataCached(cacheFileName));

    QCOMPARE(MockEepromDevice::getReadCount({testI2cDevNode, testI2cAddr}), 1);

    QByteArray written = adjRW.getData();
    QFile file(":/export_internal_initial_mt310s2.eeprom");
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray expected = file.readAll();

    QCOMPARE(expected, written);
}

void test_adj_data_cache::nocachePathCreatedOnReadValidAdj()
{
    MockEepromDevice::setData({testI2cDevNode, testI2cAddr},
                              TestLogHelpers::loadFile(":/export_internal_initial_mt310s2.eeprom"));
    MockEepromI2cFactory eepromFactory;
    EepromI2cDeviceInterfacePtr adjMemory = eepromFactory.createEeprom({testI2cDevNode, testI2cAddr},
                                                                       AbstractEepromI2cDevice::capacity24LC256);
    AdjustmentEepromReadWrite adjRW(std::move(adjMemory));
    QVERIFY(adjRW.readData());

    QVERIFY(!QDir(testPath).exists());
}

void test_adj_data_cache::nocacheFileCreatedOnReadValidAdj()
{
    MockEepromDevice::setData({testI2cDevNode, testI2cAddr},
                              TestLogHelpers::loadFile(":/export_internal_initial_mt310s2.eeprom"));
    MockEepromI2cFactory eepromFactory;
    EepromI2cDeviceInterfacePtr adjMemory = eepromFactory.createEeprom({testI2cDevNode, testI2cAddr},
                                                                       AbstractEepromI2cDevice::capacity24LC256);
    AdjustmentEepromReadWrite adjRW(std::move(adjMemory));
    QVERIFY(adjRW.readData());

    QVERIFY(!QFile(adjRW.getCacheFullFileName(cacheFileName)).exists());
}
