#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include "systeminfointerface.h"
#include "atmelctrlfactoryinterface.h"

class SystemInfo : public SystemInfoInterface
{
public:
    SystemInfo(AtmelCtrlFactoryInterfacePrt ctrlFactory);
    void getSystemInfo(); // here we fetch all system info
    bool dataRead(); // false if we were not able to read required information
    QString getDeviceVersion() override;
    QString getDeviceName() override;
    QString getPCBVersion() override;
    QString getLCAVersion() override;
    QString getCTRLVersion() override;
    QString getSerialNumber() override;

private:
    AtmelCtrlFactoryInterfacePrt m_ctrlFactory;
    bool m_bRead; // we keep in mind whether we could read the information
    QString m_sDeviceName;
    QString m_sPCBVersion;
    QString m_sLCAVersion;
    QString m_sCTRLVersion;
    QString m_sSerialNumber;
};
#endif // SYSTEMINFO_H
