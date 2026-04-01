#include "test_microcontroller_io_conversions.h"
#include "i2cctrlgenerator.h"
#include "testpcbserver.h"
#include <mocktcpnetworkfactory.h>
#include <QTest>

QTEST_MAIN(test_microcontroller_io_conversions)

void test_microcontroller_io_conversions::generatorBitmaskFromChannelMNames()
{
    TestPcbServer mock("mt310s2d", VeinTcp::MockTcpNetworkFactory::create());
    cSenseSettingsPtr senseSettings = mock.getSenseSettings();

    quint8 mask = I2cCtrlGenerator::getBitmask(senseSettings, QStringList() << "m0" << "m3");
    QCOMPARE(mask, (1<<0) | (1<<4));

    mask = I2cCtrlGenerator::getBitmask(senseSettings, QStringList() << "m6" << "m7"); // AUX
    QCOMPARE(mask, (1<<3) | (1<<7));
}

void test_microcontroller_io_conversions::generatorChannelMNamesFromBitmask()
{
    TestPcbServer mock("mt310s2d", VeinTcp::MockTcpNetworkFactory::create());
    cSenseSettingsPtr senseSettings = mock.getSenseSettings();

    QStringList channelMNames = I2cCtrlGenerator::getChannelMNamesFromMask(senseSettings, (1<<0) | (1<<4));
    QCOMPARE(channelMNames.size(), 2);
    QVERIFY(channelMNames.contains("m0"));
    QVERIFY(channelMNames.contains("m3"));

    channelMNames = I2cCtrlGenerator::getChannelMNamesFromMask(senseSettings, (1<<3) | (1<<7));
    QCOMPARE(channelMNames.size(), 2);
    QVERIFY(channelMNames.contains("m6"));
    QVERIFY(channelMNames.contains("m7"));
}

void test_microcontroller_io_conversions::controllerInternalChannelNoFromChannelMName()
{
    TestPcbServer mock("mt310s2d", VeinTcp::MockTcpNetworkFactory::create());
    cSenseSettingsPtr senseSettings = mock.getSenseSettings();

    quint8 channelNo = I2cCtrlGenerator::getControllerInternalChannelNo(senseSettings, "m0");
    QCOMPARE(channelNo, 1);

    channelNo = I2cCtrlGenerator::getControllerInternalChannelNo(senseSettings, "m3");
    QCOMPARE(channelNo, 5);

    channelNo = I2cCtrlGenerator::getControllerInternalChannelNo(senseSettings, "m6");
    QCOMPARE(channelNo, 4);

    channelNo = I2cCtrlGenerator::getControllerInternalChannelNo(senseSettings, "m7");
    QCOMPARE(channelNo, 8);
}
