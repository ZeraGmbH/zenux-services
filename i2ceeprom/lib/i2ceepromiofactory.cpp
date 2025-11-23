#include "i2ceepromiofactory.h"
#include "eepromi2c_24lcxxx.h"

const I2cEEpromIoFactory::Creator I2cEEpromIoFactory::m_createFunctionDefault = []
    (const I2cAddressParameter &i2cAddressParam, int byteCapacity) {
    return std::make_unique<EepromI2c_24LCxxx>(i2cAddressParam, byteCapacity);
};

I2cEEpromIoFactory::Creator I2cEEpromIoFactory::m_createFunction = m_createFunctionDefault;

EepromI2cDeviceInterfacePtr I2cEEpromIoFactory::create24LcTypeEeprom(const I2cAddressParameter &i2cAddressParam,
                                                               int byteCapacity)
{
    return m_createFunction(i2cAddressParam, byteCapacity);
}
