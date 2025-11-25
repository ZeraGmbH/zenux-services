#ifndef ABSTRACTEEPROMI2CFACTORY_H
#define ABSTRACTEEPROMI2CFACTORY_H

#include "abstracteepromi2cdevice.h"
#include <i2caddressparameter.h>
#include <memory>

class AbstractEepromI2cFactory
{
public:
    virtual ~AbstractEepromI2cFactory() = default;
    virtual EepromI2cDeviceInterfacePtr createEeprom(const I2cAddressParameter &i2cAddress, int byteCapacity) = 0;
    virtual EepromI2cDeviceInterfacePtr createEepromOnMux(const I2cAddressParameter &i2cAddress, int byteCapacity,
                                                          const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo) = 0;
};

typedef std::shared_ptr<AbstractEepromI2cFactory> AbstractEepromI2cFactoryPtr;

#endif // ABSTRACTEEPROMI2CFACTORY_H
