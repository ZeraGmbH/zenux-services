#ifndef ABSTRACTEEPROMI2CDEVICE_H
#define ABSTRACTEEPROMI2CDEVICE_H

#include <QString>
#include <memory>

class AbstractEepromI2cDevice
{
public:
    static constexpr int capacity24LC256 = (1 << 15);
    static constexpr int capacity24LC512 = (1 << 16);

    virtual ~AbstractEepromI2cDevice() = default;
    virtual bool isMemoryPlugged() const = 0;
    virtual int WriteData(char* data, ushort count, ushort adr) = 0;
    virtual int ReadData(char* data, ushort count, ushort adr) = 0;
    virtual int Reset() = 0;
    virtual int getByteSize() const = 0;
};

typedef std::unique_ptr<AbstractEepromI2cDevice> EepromI2cDeviceInterfacePtr;

#endif // ABSTRACTEEPROMI2CDEVICE_H
