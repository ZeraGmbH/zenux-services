#ifndef I2CFLASHIOFACTORY_H
#define I2CFLASHIOFACTORY_H

#include "i2cflashinterface.h"
#include <QString>
#include <functional>

class I2cFlashIoFactory
{
public:
    static I2cFlashInterfacePtrU create24LC256(QString devNode, short i2cAddr);
protected:
    static const std::function<I2cFlashInterfacePtrU(QString devNode, short i2cAddr)> m_cresteFunctionDefault;
    static std::function<I2cFlashInterfacePtrU(QString devNode, short i2cAddr)> m_cresteFunction;
};

#endif // I2CFLASHIOFACTORY_H
