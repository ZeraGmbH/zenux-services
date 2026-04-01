#ifndef I2CUTILITIES_H
#define I2CUTILITIES_H

#include <QByteArray>

class I2cUtilities
{
public:
    static QByteArray convertFloat(float value);
    static float unconvertFloat(const QByteArray& binFloat);
};

#endif // I2CUTILITIES_H
