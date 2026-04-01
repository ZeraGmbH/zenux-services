#ifndef I2CDSPGENERATOR_H
#define I2CDSPGENERATOR_H

#include <QByteArray>
#include <QDataStream>

class I2cDspGenerator
{
public:
    static QByteArray getCmdSetAmplitude(float amplitude);
    static QByteArray getCmdGetAmplitude();
    static QByteArray getCmdSetFrequency(float frequency);
    static QByteArray getCmdGetFrequency();

private:
    static QByteArray createSetterFloat(quint16 cmdId, float value);
    static QByteArray createGetterFloat(quint16 cmdId);
};

#endif // I2CDSPGENERATOR_H
