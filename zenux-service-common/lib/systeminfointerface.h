#ifndef SYSTEMINFOINTERFACE_H
#define SYSTEMINFOINTERFACE_H

#include <QString>

class SystemInfoInterface
{
public:
    virtual QString getDeviceVersion() = 0;
    virtual QString getDeviceName() = 0;
    virtual QString getPCBVersion() = 0;
    virtual QString getLCAVersion() = 0;
    virtual QString getCTRLVersion() = 0;
    virtual QString getSerialNumber() = 0;
};

#endif // SYSTEMINFOINTERFACE_H
