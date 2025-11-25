#ifndef EEPROMI2C24LCXXX_H
#define EEPROMI2C24LCXXX_H

#include "abstracteepromi2cdevice.h"
#include "i2caddressparameter.h"
#include <QString>

class EepromI2c24LCxxx : public AbstractEepromI2cDevice
{
public:
    EepromI2c24LCxxx(const I2cAddressParameter &i2cAddressParam, int byteCapacity);
    bool isMemoryPlugged() const override;
    int WriteData(char* data, ushort count, ushort memAddress) override;
    int ReadData(char* data, ushort count, ushort memAddress) override;
    int Reset() override;
    int getByteSize() const override;

private:
    const I2cAddressParameter m_i2cAddressParam;
    const int m_byteCapacity;
};

#endif // EEPROMI2C24LCXXX_H
