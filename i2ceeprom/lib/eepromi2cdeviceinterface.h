#ifndef EEPROMI2CDEVICEINTERFACE_H
#define EEPROMI2CDEVICEINTERFACE_H

#include <QString>
#include <memory>

class EepromI2cDeviceInterface
{
public:
    static constexpr int capacity24LC256 = (1 << 15);
    static constexpr int capacity24LC512 = (1 << 16);

    virtual ~EepromI2cDeviceInterface() = default;
    virtual int WriteData(char* data, ushort count, ushort adr) = 0;
    virtual int ReadData(char* data, ushort count, ushort adr) = 0;
    virtual int Reset() = 0;
    virtual int getByteSize() const = 0;
};

typedef std::unique_ptr<EepromI2cDeviceInterface> EepromI2cDeviceInterfacePtr;

#endif // EEPROMI2CDEVICEINTERFACE_H
