#ifndef I2CDSPGENERATOR_H
#define I2CDSPGENERATOR_H

#include <QByteArray>
#include <QDataStream>

class I2cDspGenerator
{
public:
    static QByteArray getCmdSetAmplitude(float amplitude);
    static QByteArray getCmdGetAmplitude();
};

#endif // I2CDSPGENERATOR_H
