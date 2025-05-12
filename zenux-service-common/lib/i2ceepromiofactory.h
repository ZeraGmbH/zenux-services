#ifndef I2CEEPROMIOFACTORY_H
#define I2CEEPROMIOFACTORY_H

#include "i2cflashinterface.h"
#include <QString>
#include <functional>

class I2cEEpromIoFactory
{
public:
    static I2cFlashInterfacePtrU create24LcTypeEeprom(QString devNode, short i2cAddr);
protected:
    static const std::function<I2cFlashInterfacePtrU(QString devNode, short i2cAddr)> m_createFunctionDefault;
    static std::function<I2cFlashInterfacePtrU(QString devNode, short i2cAddr)> m_createFunction;
};

#endif // I2CEEPROMIOFACTORY_H
