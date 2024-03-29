#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <QString>

class Sec1000SystemInfo
{
public:
    Sec1000SystemInfo();
    void getSystemInfo(); // here we fetch all system info
    bool dataRead(); // false if we were not able to read required information
    QString getDeviceVersion();
    QString& getDeviceName();
    QString& getPCBVersion();
    QString& getLCAVersion();
    QString& getSerialNumber();

private:
    bool m_bRead; // we keep in mind whether we could read the information
    QString m_sDeviceName;
    QString m_sPCBVersion;
    QString m_sLCAVersion;
    QString m_sSerialNumber;
};
#endif // SYSTEMINFO_H
