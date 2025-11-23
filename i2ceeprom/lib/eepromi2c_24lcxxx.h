#ifndef EEPROMI2C_24LCXXX_H
#define EEPROMI2C_24LCXXX_H

#include "eepromi2cdeviceinterface.h"
#include "i2caddressparameter.h"
#include "zera-i2c-devices_export.h"
#include <QString>

class ZERA_I2C_DEVICES_EXPORT EepromI2c_24LCxxx : public EepromI2cDeviceInterface
{
public:
    EepromI2c_24LCxxx(const I2cAddressParameter &i2cAddressParam, int byteCapacity);
    int WriteData(char* data, ushort count, ushort memAddress) override;
    int ReadData(char* data, ushort count, ushort memAddress) override;
    int Reset() override;
    int getByteSize() const override;

private:
    const I2cAddressParameter m_i2cAddressParam;
    const int m_byteCapacity;
};

#endif // EEPROMI2C_24LCXXX_H
