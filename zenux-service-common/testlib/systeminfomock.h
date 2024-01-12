#ifndef SYSTEMINFOMOCK_H
#define SYSTEMINFOMOCK_H

#include <systeminfo.h>

class SystemInfoMock : public SystemInfo
{
public:
    SystemInfoMock(AtmelCtrlFactoryInterfacePrt ctrlFactory);
    QString getDeviceName() override;
    QString getPCBVersion() override;
    QString getLCAVersion() override;
    QString getCTRLVersion() override;
    QString getSerialNumber() override;

    void setDeviceName(QString name);
    void setPCBVersion(QString version);
    void setLCAVersion(QString version);
    void setCTRLVersion(QString version);
    void setSerialNumber(QString serno);

private:
    QString m_deviceName = "Unknown";
    QString m_pcpVersion = "Unknown";
    QString m_lcaVersion = "Unknown";
    QString m_ctrlVersion = "Unknown";
    QString m_serialNumber = "Unknown";
};

#endif // SYSTEMINFOMOCK_H
