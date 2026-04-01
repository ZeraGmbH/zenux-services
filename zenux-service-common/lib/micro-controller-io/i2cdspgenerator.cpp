#include "i2cdspgenerator.h"

enum hw_cmdcode
{
    hwSendGetAmplitude = 0xA005,
    hwSendSetAmplitude = 0xA006,
    hwSendGetFrequency = 0xA00B,
    hwSendSetFrequency = 0xA00C,
};

QByteArray I2cDspGenerator::getCmdGetAmplitude()
{
    return createGetterFloat(hwSendGetAmplitude);
}

QByteArray I2cDspGenerator::getCmdSetAmplitude(float amplitude)
{
    return createSetterFloat(hwSendSetAmplitude, amplitude);
}

QByteArray I2cDspGenerator::getCmdGetFrequency()
{
    return createGetterFloat(hwSendGetFrequency);
}

QByteArray I2cDspGenerator::getCmdSetFrequency(float frequency)
{
    return createSetterFloat(hwSendSetFrequency, frequency);
}

QByteArray I2cDspGenerator::createSetterFloat(quint16 cmdId, float value)
{
    QByteArray ret;
    QDataStream stream(&ret, QIODevice::Unbuffered | QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    stream << cmdId;
    stream << value;

    return ret;
}

QByteArray I2cDspGenerator::createGetterFloat(quint16 cmdId)
{
    QByteArray ret;
    QDataStream stream(&ret, QIODevice::Unbuffered | QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    stream << cmdId;

    return ret;
}

