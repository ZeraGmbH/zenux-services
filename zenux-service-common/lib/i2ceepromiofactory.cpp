#include "i2ceepromiofactory.h"
#include <eepromi2c_24lcxxx.h>

const I2cEEpromIoFactory::Creator I2cEEpromIoFactory::m_createFunctionDefault = []
    (QString devNode, short i2cAddr, int byteCapacity) {
    return std::make_unique<EepromI2c_24LCxxx>(devNode, i2cAddr, byteCapacity);
};

I2cEEpromIoFactory::Creator I2cEEpromIoFactory::m_createFunction = m_createFunctionDefault;

I2cFlashInterfacePtrU I2cEEpromIoFactory::create24LcTypeEeprom(QString devNode, short i2cAddr, int byteCapacity)
{
    return m_createFunction(devNode, i2cAddr, byteCapacity);
}
