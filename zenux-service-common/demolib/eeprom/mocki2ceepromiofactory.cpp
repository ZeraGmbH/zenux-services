#include "mocki2ceepromiofactory.h"
#include "mockeeprom24lc.h"

void MockI2cEEpromIoFactory::enableMock()
{
    m_createFunction = [](QString devNode, short i2cAddr, int byteCapacity) {
        return std::make_unique<MockEEprom24LC>(devNode, i2cAddr, byteCapacity);
    };
}

void MockI2cEEpromIoFactory::disableMock()
{
    m_createFunction = m_createFunctionDefault;
}
