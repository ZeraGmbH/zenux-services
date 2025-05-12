#include "i2ceepromiofactory.h"
#include <F24LC256.h>

const std::function<I2cFlashInterfacePtrU(QString devNode, short i2cAddr)> I2cEEpromIoFactory::m_createFunctionDefault = [](QString devNode, short i2cAddr) {
    return std::make_unique<cF24LC256>(devNode, i2cAddr);
};

std::function<I2cFlashInterfacePtrU(QString devNode, short i2cAddr)> I2cEEpromIoFactory::m_createFunction = m_createFunctionDefault;

I2cFlashInterfacePtrU I2cEEpromIoFactory::create24LcTypeEeprom(QString devNode, short i2cAddr)
{
    return m_createFunction(devNode, i2cAddr);
}
