#include "i2cutilities.h"
#include <QDataStream>

QByteArray I2cUtilities::convertFloat(float value)
{
    QByteArray memOut;
    QDataStream stream(&memOut, QIODevice::Unbuffered | QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream << value;
    return memOut;
}

float I2cUtilities::unconvertFloat(const QByteArray &binFloat)
{
    QByteArray localfloat = binFloat;
    QDataStream stream(&localfloat, QIODevice::Unbuffered | QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    float ret = 0.0;
    stream >> ret;
    return ret;
}
