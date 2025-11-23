#include "mocki2ceepromiofactory.h"
#include "mockeeprom24lc.h"

void MockI2cEEpromIoFactory::enableMock()
{
    m_createFunction = [](const I2cAddressParameter &i2cAddress, int byteCapacity) {
        return std::make_unique<MockEEprom24LC>(i2cAddress, byteCapacity);
    };
}

void MockI2cEEpromIoFactory::disableMock()
{
    m_createFunction = m_createFunctionDefault;
}
