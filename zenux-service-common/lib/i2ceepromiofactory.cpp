#include "i2ceepromiofactory.h"
#include <F24LC256.h>

const std::function<I2cFlashInterfacePtrU(QString devNode, short i2cAddr)> I2cEEpromIoFactory::m_cresteFunctionDefault = [](QString devNode, short i2cAddr) {
    return std::make_unique<cF24LC256>(devNode, i2cAddr);
};

std::function<I2cFlashInterfacePtrU(QString devNode, short i2cAddr)> I2cEEpromIoFactory::m_cresteFunction = m_cresteFunctionDefault;

I2cFlashInterfacePtrU I2cEEpromIoFactory::create24LC256(QString devNode, short i2cAddr)
{
    return m_cresteFunction(devNode, i2cAddr);
}
