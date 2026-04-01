#include "test_i2c_dspgenerator.h"
#include "i2cdspgenerator.h"
#include <QTest>

QTEST_MAIN(test_i2c_dspgenerator);

void test_i2c_dspgenerator::getAmplitudeCmd()
{
    QByteArray cmd = I2cDspGenerator::getCmdGetAmplitude();
    QCOMPARE(cmd.size(), 2);
    QCOMPARE(cmd[0], char(0xA0));
    QCOMPARE(cmd[1], char(0x05));
}

void test_i2c_dspgenerator::setAmplitudeCmd()
{
    QByteArray cmd = I2cDspGenerator::getCmdSetAmplitude(22.7);
    QCOMPARE(cmd.size(), 6);
    QCOMPARE(cmd[0], char(0xA0));
    QCOMPARE(cmd[1], char(0x06));
    QCOMPARE(cmd[2], char(0x41));
    QCOMPARE(cmd[3], char(0xB5));
    QCOMPARE(cmd[4], char(0x99));
    QCOMPARE(cmd[5], char(0x9A));
}

void test_i2c_dspgenerator::getFrequencyCmd()
{
    QByteArray cmd = I2cDspGenerator::getCmdGetFrequency();
    QCOMPARE(cmd.size(), 2);
    QCOMPARE(cmd[0], char(0xa0));
    QCOMPARE(cmd[1], char(0x0B));
}

void test_i2c_dspgenerator::setFrequencyCmd()
{
    QByteArray cmd = I2cDspGenerator::getCmdSetFrequency(50);
    QCOMPARE(cmd.size(), 6);
    QCOMPARE(cmd[0], char(0xa0));
    QCOMPARE(cmd[1], char(0x0C));
    QCOMPARE(cmd[2], char(0x42));
    QCOMPARE(cmd[3], char(0x48));
    QCOMPARE(cmd[4], char(0x00));
    QCOMPARE(cmd[5], char(0x00));
}
