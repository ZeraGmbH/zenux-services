#include "i2cflashiofactoryfortest.h"
#include "eeprom24lcmock.h"

void I2cFlashIoFactoryForTest::enableMockFlash()
{
    m_cresteFunction = [](QString devNode, short i2cAddr) {
        return std::make_unique<EEprom24LCMock>(devNode, i2cAddr);
    };
}

void I2cFlashIoFactoryForTest::disableMockFlash()
{
    m_cresteFunction = m_cresteFunctionDefault;
}
