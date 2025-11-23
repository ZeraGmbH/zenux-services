#include "mockeepromi2cfactory.h"
#include "mockeeprom24lc.h"

QHash<EepromWithMuxParams, QByteArray> MockEepromI2cFactory::m_flashData;
QHash<EepromWithMuxParams, int>        MockEepromI2cFactory::m_flashDataReadCounts;
QHash<EepromWithMuxParams, int>        MockEepromI2cFactory::m_flashDataWriteCounts;
QHash<EepromWithMuxParams, bool>       MockEepromI2cFactory::m_returnReducedDataSizeOnRead;

EepromI2cDeviceInterfacePtr MockEepromI2cFactory::createEeprom(const I2cAddressParameter &i2cAddress, int byteCapacity)
{
    return std::make_unique<MockEEprom24LC>(i2cAddress,
                                            byteCapacity);
}

EepromI2cDeviceInterfacePtr MockEepromI2cFactory::createEepromOnMux(const I2cAddressParameter &i2cAddress, int byteCapacity,
                                                                    const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo)
{
    return std::make_unique<MockEEprom24LC>(i2cAddress,
                                            byteCapacity,
                                            i2cAddressMux,
                                            muxChannelNo);
}
