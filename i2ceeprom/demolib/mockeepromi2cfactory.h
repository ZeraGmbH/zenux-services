#ifndef MOCKEEPROMI2CFACTORY_H
#define MOCKEEPROMI2CFACTORY_H

#include <abstracteepromi2cfactory.h>
#include <QHash>

struct EepromWithMuxParams {
    I2cAddressParameter m_i2cAddress;
    I2cAddressParameter m_i2cAddressMux;
    qint8 m_muxChannelNo;
};

class MockEepromI2cFactory : public AbstractEepromI2cFactory
{
public:
    EepromI2cDeviceInterfacePtr createEeprom(const I2cAddressParameter &i2cAddress, int byteCapacity) override;
    EepromI2cDeviceInterfacePtr createEepromOnMux(const I2cAddressParameter &i2cAddress, int byteCapacity,
                                                  const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo) override;
};

#endif // MOCKEEPROMI2CFACTORY_H
