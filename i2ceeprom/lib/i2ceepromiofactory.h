#ifndef I2CEEPROMIOFACTORY_H
#define I2CEEPROMIOFACTORY_H

#include "eepromi2cdeviceinterface.h"
#include "i2caddressparameter.h"
#include <QString>
#include <functional>

class I2cEEpromIoFactory
{
public:
    static EepromI2cDeviceInterfacePtr create24LcTypeEeprom(const I2cAddressParameter &i2cAddressParam, int byteCapacity);
protected:
    typedef std::function<EepromI2cDeviceInterfacePtr(const I2cAddressParameter &i2cAddressParam, int byteCapacity)> Creator;
    static const Creator m_createFunctionDefault;
    static Creator m_createFunction;
};

#endif // I2CEEPROMIOFACTORY_H
