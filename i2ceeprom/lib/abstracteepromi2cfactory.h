#ifndef ABSTRACTEEPROMI2CFACTORY_H
#define ABSTRACTEEPROMI2CFACTORY_H

#include "eepromi2cdeviceinterface.h"
#include <i2caddressparameter.h>

class AbstractEepromI2cFactory
{
public:
    virtual ~AbstractEepromI2cFactory() = default;
    virtual EepromI2cDeviceInterfacePtr createEeprom(const I2cAddressParameter &i2cAddress, int byteCapacity) = 0;
    virtual EepromI2cDeviceInterfacePtr createEepromOnMux(const I2cAddressParameter &i2cAddress, int byteCapacity,
                                                          const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo) = 0;
};

#endif // ABSTRACTEEPROMI2CFACTORY_H
