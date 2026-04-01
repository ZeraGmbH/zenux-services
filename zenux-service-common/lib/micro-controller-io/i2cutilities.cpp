#include "i2cutilities.h"
#include <QDataStream>

QByteArray I2cUtilities::convertFloat(float value)
{
    QByteArray memOut;
    QDataStream stream(&memOut, QIODevice::Unbuffered | QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream<<value;
    return memOut;
}
