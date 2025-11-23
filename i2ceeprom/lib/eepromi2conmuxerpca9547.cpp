#include "eepromi2conmuxerpca9547.h"
#include <i2cmuxerpca9547.h>
#include <i2cmuxerscopedonoff.h>

EepromI2cOnMuxerPCA9547::EepromI2cOnMuxerPCA9547(EepromI2cDeviceInterfacePtr eeprom,
                                                 const I2cAddressParameter &i2cAddressMux, qint8 muxChannelNo) :
    m_eeprom(std::move(eeprom)),
    m_i2cMuxer(std::make_shared<I2cMuxerPCA9547>(i2cAddressMux.devNodeFileName, i2cAddressMux.i2cAddr, muxChannelNo))
{
}

int EepromI2cOnMuxerPCA9547::WriteData(char *data, ushort count, ushort memAddress)
{
    I2cMuxerScopedOnOff i2cMuxOnOff(m_i2cMuxer);
    return m_eeprom->WriteData(data, count, memAddress);
}

int EepromI2cOnMuxerPCA9547::ReadData(char *data, ushort count, ushort memAddress)
{
    I2cMuxerScopedOnOff i2cMuxOnOff(m_i2cMuxer);
    return m_eeprom->ReadData(data, count, memAddress);
}

int EepromI2cOnMuxerPCA9547::Reset()
{
    I2cMuxerScopedOnOff i2cMuxOnOff(m_i2cMuxer);
    return m_eeprom->Reset();
}

int EepromI2cOnMuxerPCA9547::getByteSize() const
{
    return m_eeprom->getByteSize();
}
