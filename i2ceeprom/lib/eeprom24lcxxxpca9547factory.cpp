#include "eeprom24lcxxxpca9547factory.h"
#include "eepromi2c24lcxxx.h"
#include "eepromi2conmuxerpca9547.h"

EepromI2cDeviceInterfacePtr Eeprom24LCxxxPca9547Factory::createEeprom(const I2cAddressParameter &i2cAddress, int byteCapacity)
{
    return std::make_unique<EepromI2c24LCxxx>(i2cAddress, byteCapacity);
}

EepromI2cDeviceInterfacePtr Eeprom24LCxxxPca9547Factory::createEepromOnMux(const I2cAddressParameter &i2cAddress, int byteCapacity,
                                                                           const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    return std::make_unique<EepromI2cOnMuxerPCA9547>(
        std::make_unique<EepromI2c24LCxxx>(i2cAddress, byteCapacity),
        i2cAddressMux, muxChannelNo);
}
