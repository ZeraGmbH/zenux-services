#include "test_i2c_utilities.h"
#include "i2cutilities.h"
#include <QTest>

QTEST_MAIN(test_i2c_utilities)

// Conversions created / checked in [1]
// [1] https://www.h-schmidt.net/FloatConverter/IEEE754.html

void test_i2c_utilities::serializeFloat()
{
    QByteArray binFloat = I2cUtilities::convertFloat(50.11);
    QCOMPARE(binFloat.size(), 4);
    QCOMPARE(binFloat[0], char(0x42));
    QCOMPARE(binFloat[1], char(0x48));
    QCOMPARE(binFloat[2], char(0x70));
    QCOMPARE(binFloat[3], char(0xa4));
}

void test_i2c_utilities::deserializeFloat()
{
    QByteArray binFloat;
    binFloat.append(0x42);
    binFloat.append(0x48);
    binFloat.append(0x70);
    binFloat.append(0xa4);

    float value = I2cUtilities::unconvertFloat(binFloat);
    QCOMPARE(value, 50.1100006103515625);
}
