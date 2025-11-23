#include "mockeepromi2cfactory.h"
#include "mockeepromdevice.h"

EepromI2cDeviceInterfacePtr MockEepromI2cFactory::createEeprom(const I2cAddressParameter &i2cAddress, int byteCapacity)
{
    return std::make_unique<MockEepromDevice>(i2cAddress,
                                            byteCapacity);
}

EepromI2cDeviceInterfacePtr MockEepromI2cFactory::createEepromOnMux(const I2cAddressParameter &i2cAddress, int byteCapacity,
                                                                    const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    return std::make_unique<MockEepromDevice>(i2cAddress,
                                            byteCapacity,
                                            i2cAddressMux,
                                            muxChannelNo);
}
