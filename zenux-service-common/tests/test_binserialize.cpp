#include "test_binserialize.h"
#include "i2cctrlgenerator.h"
#include <QTest>

QTEST_MAIN(test_binserialize)

void test_binserialize::serializeFloatToMicroController()
{
    QByteArray mem = I2cCtrlGenerator::convertFloat(50.11);
    QCOMPARE(mem.size(), 4);
    QCOMPARE(mem[0], char(0x42));
    QCOMPARE(mem[1], char(0x48));
    QCOMPARE(mem[2], char(0x70));
    QCOMPARE(mem[3], char(0xa4));
}
