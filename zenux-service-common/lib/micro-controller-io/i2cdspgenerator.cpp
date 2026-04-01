#include "i2cdspgenerator.h"

enum hw_cmdcode
{
    hwSendGetAmplitude = 0xA005,
    hwSendSetAmplitude = 0xA006,
};

QByteArray I2cDspGenerator::getCmdGetAmplitude()
{
    QByteArray ret;
    QDataStream stream(&ret, QIODevice::Unbuffered | QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    stream << quint16(hwSendGetAmplitude);

    return ret;
}

QByteArray I2cDspGenerator::getCmdSetAmplitude(float amplitude)
{
    QByteArray ret;
    QDataStream stream(&ret, QIODevice::Unbuffered | QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    stream << quint16(hwSendSetAmplitude);
    stream << amplitude;

    return ret;
}

