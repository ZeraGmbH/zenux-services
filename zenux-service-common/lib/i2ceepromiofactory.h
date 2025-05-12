#ifndef I2CEEPROMIOFACTORY_H
#define I2CEEPROMIOFACTORY_H

#include "eepromi2cdeviceinterface.h"
#include <QString>
#include <functional>

class I2cEEpromIoFactory
{
public:
    static I2cFlashInterfacePtrU create24LcTypeEeprom(QString devNode, short i2cAddr, int byteCapacity);
protected:
    typedef std::function<I2cFlashInterfacePtrU(QString devNode, short i2cAddr, int byteCapacity)> Creator;
    static const Creator m_createFunctionDefault;
    static Creator m_createFunction;
};

#endif // I2CEEPROMIOFACTORY_H
