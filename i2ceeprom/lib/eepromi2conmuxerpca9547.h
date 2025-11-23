#ifndef EEPROMI2CONMUXERPCA9547_H
#define EEPROMI2CONMUXERPCA9547_H

#include "eepromi2cdeviceinterface.h"
#include <i2caddressparameter.h>
#include <i2cmuxerinterface.h>

// EepromI2cDevice decorator
class EepromI2cOnMuxerPCA9547 : public EepromI2cDeviceInterface
{
public:
    EepromI2cOnMuxerPCA9547(EepromI2cDeviceInterfacePtr eeprom,
                            const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo);
    int WriteData(char* data, ushort count, ushort memAddress) override;
    int ReadData(char* data, ushort count, ushort memAddress) override;
    int Reset() override;
    int getByteSize() const override;
private:
    EepromI2cDeviceInterfacePtr m_eeprom;
    I2cMuxerInterface::Ptr m_i2cMuxer;
};

#endif // EEPROMI2CONMUXERPCA9547_H
