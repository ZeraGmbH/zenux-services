#ifndef EEPROM24LCXXXPCA9547FACTORY_H
#define EEPROM24LCXXXPCA9547FACTORY_H

#include "abstracteepromi2cfactory.h"

class Eeprom24LCxxxPca9547Factory : public AbstractEepromI2cFactory
{
public:
    EepromI2cDeviceInterfacePtr createEeprom(const I2cAddressParameter &i2cAddress, int byteCapacity) override;
    EepromI2cDeviceInterfacePtr createEepromOnMux(const I2cAddressParameter &i2cAddress, int byteCapacity,
                                                  const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo) override;
};

#endif // EEPROM24LCXXXPCA9547FACTORY_H
