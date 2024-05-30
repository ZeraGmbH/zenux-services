#ifndef MT310S2SYSTEMINFOMOCK_H
#define MT310S2SYSTEMINFOMOCK_H

#include <mt310s2systeminfo.h>

class Mt310s2SystemInfoMock : public Mt310s2SystemInfo
{
public:
    Mt310s2SystemInfoMock(AbstractFactoryI2cCtrlPtr ctrlFactory);

    QString getDeviceName() override;
    QString getPCBVersion() override;
    QString getLCAVersion() override;
    QString getCTRLVersion() override;
    QString getSerialNumber() override;

    static void setDeviceName(QString name);
    static void setPCBVersion(QString version);
    static void setLCAVersion(QString version);
    static void setCTRLVersion(QString version);
    static void setSerialNumber(QString serno);

    QString getSysCTRLVersion() override;
    QString getSysPCBVersion() override;

private:
    static QString m_deviceName;
    static QString m_pcpVersion;
    static QString m_FPGAVersion;
    static QString m_ctrlVersion;
    static QString m_serialNumber;
};

#endif // MT310S2SYSTEMINFOMOCK_H
