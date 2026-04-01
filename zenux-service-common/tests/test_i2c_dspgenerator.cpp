#include "test_i2c_dspgenerator.h"
#include "i2cdspgenerator.h"
#include <QTest>

QTEST_MAIN(test_i2c_dspgenerator);

void test_i2c_dspgenerator::getAmplitudeCmd()
{
    QByteArray cmd = I2cDspGenerator::getCmdGetAmplitude();
    QCOMPARE(cmd.size(), 2);
    QCOMPARE(cmd[0], char(0xa0));
    QCOMPARE(cmd[1], char(0x05));
}

void test_i2c_dspgenerator::setAmplitudeCmd()
{
    QByteArray cmd = I2cDspGenerator::getCmdSetAmplitude(22.7);
    QCOMPARE(cmd.size(), 6);
    QCOMPARE(cmd[0], char(0xa0));
    QCOMPARE(cmd[1], char(0x06));
    QCOMPARE(cmd[2], char(0x41));
    QCOMPARE(cmd[3], char(0xb5));
    QCOMPARE(cmd[4], char(0x99));
    QCOMPARE(cmd[5], char(0x9a));
}
