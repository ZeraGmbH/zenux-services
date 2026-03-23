#include "test_microcontroller_io_conversions.h"
#include "i2cctrlgenerator.h"
#include "testpcbserver.h"
#include <mocktcpnetworkfactory.h>
#include <QTest>

QTEST_MAIN(test_microcontroller_io_conversions)

void test_microcontroller_io_conversions::serializeFloatToMicroController()
{
    QByteArray mem = I2cCtrlGenerator::convertFloat(50.11);
    QCOMPARE(mem.size(), 4);
    QCOMPARE(mem[0], char(0x42));
    QCOMPARE(mem[1], char(0x48));
    QCOMPARE(mem[2], char(0x70));
    QCOMPARE(mem[3], char(0xa4));
}

void test_microcontroller_io_conversions::generatorBitmaskFromChannelMNames()
{
    TestPcbServer mock("mt310s2d", VeinTcp::MockTcpNetworkFactory::create());
    cSenseSettingsPtr senseSettings = mock.getSenseSettings();

    quint8 mask = I2cCtrlGenerator::getBitmask(senseSettings, QStringList() << "m0" << "m3");
    QCOMPARE(mask, (1<<0) | (1<<4));
    mask = I2cCtrlGenerator::getBitmask(senseSettings, QStringList() << "m6" << "m7"); // AUX
    QCOMPARE(mask, (1<<3) | (1<<7));
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
