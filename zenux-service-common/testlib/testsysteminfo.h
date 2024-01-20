#ifndef TESTSYSTEMINFO_H
#define TESTSYSTEMINFO_H

#include <systeminfo.h>

class TestSystemInfo : public SystemInfo
{
public:
    TestSystemInfo(AbstractFactoryI2cCtrlPtr ctrlFactory);
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
    QString m_FPGAVersion = "Unknown";
    QString m_ctrlVersion = "Unknown";
    QString m_serialNumber = "Unknown";
};

#endif // TESTSYSTEMINFO_H
