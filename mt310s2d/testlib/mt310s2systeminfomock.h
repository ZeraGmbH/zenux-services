#ifndef MT310S2SYSTEMINFOMOCK_H
#define MT310S2SYSTEMINFOMOCK_H

#include <mt310s2systeminfo.h>

class Mt310s2SystemInfoMock : public Mt310s2SystemInfo
{
public:
    Mt310s2SystemInfoMock(AtmelCtrlFactoryInterfacePrt ctrlFactory);

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

    QString getSysCTRLVersion() override;
    QString getSysPCBVersion() override;

private:
    QString m_deviceName = "Unknown";
    QString m_pcpVersion = "Unknown";
    QString m_lcaVersion = "Unknown";
    QString m_ctrlVersion = "Unknown";
    QString m_serialNumber = "Unknown";
};

#endif // MT310S2SYSTEMINFOMOCK_H
