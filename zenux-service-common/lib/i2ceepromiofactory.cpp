#include "i2ceepromiofactory.h"
#include <eepromi2c_24lcxxx.h>

const I2cEEpromIoFactory::Creator I2cEEpromIoFactory::m_createFunctionDefault = []
    (const EepromI2cDeviceInterface::AddressData &addressData, int byteCapacity) {
    return std::make_unique<EepromI2c_24LCxxx>(addressData, byteCapacity);
};

I2cEEpromIoFactory::Creator I2cEEpromIoFactory::m_createFunction = m_createFunctionDefault;

I2cFlashInterfacePtrU I2cEEpromIoFactory::create24LcTypeEeprom(const EepromI2cDeviceInterface::AddressData &addressData,
                                                               int byteCapacity)
{
    return m_createFunction(addressData, byteCapacity);
}
