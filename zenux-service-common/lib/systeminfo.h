#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include "abstractfactoryi2cctrl.h"

class SystemInfo
{
public:
    SystemInfo(FactoryControllerAbstractPtr ctrlFactory);
    virtual ~SystemInfo() = default;
    void getSystemInfo(); // here we fetch all system info
    bool dataRead(); // false if we were not able to read required information
    QString getDeviceVersion();
    virtual QString getDeviceName();
    virtual QString getPCBVersion();
    virtual QString getLCAVersion();
    virtual QString getCTRLVersion();
    virtual QString getSerialNumber();

private:
    FactoryControllerAbstractPtr m_ctrlFactory;
    bool m_bRead; // we keep in mind whether we could read the information
    QString m_sDeviceName;
    QString m_sPCBVersion;
    QString m_sLCAVersion;
    QString m_sCTRLVersion;
    QString m_sSerialNumber;
};
#endif // SYSTEMINFO_H
