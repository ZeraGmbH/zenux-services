#include "i2cflashiofactoryfortest.h"
#include "flash24lc256mock.h"

void I2cFlashIoFactoryForTest::enableMockFlash()
{
    m_cresteFunction = [](QString devNode, short i2cAddr) {
        return std::make_unique<Flash24LC256Mock>(devNode, i2cAddr);
    };
}

void I2cFlashIoFactoryForTest::disableMockFlash()
{
    m_cresteFunction = m_cresteFunctionDefault;
}
