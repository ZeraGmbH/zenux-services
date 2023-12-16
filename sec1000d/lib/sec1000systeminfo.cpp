#include "sec1000systeminfo.h"

Sec1000SystemInfo::Sec1000SystemInfo()
{
    m_sDeviceName = m_sPCBVersion = m_sLCAVersion = m_sSerialNumber = "Unknown";
    getSystemInfo();
}


void Sec1000SystemInfo::getSystemInfo()
{
    m_bRead = true;
}


bool Sec1000SystemInfo::dataRead()
{
    return m_bRead;
}


QString Sec1000SystemInfo::getDeviceVersion()
{
    QString s = QString ("DEVICE: %1;PCB: %2;LCA: %3")
            .arg(m_sDeviceName)
            .arg(m_sPCBVersion)
            .arg(m_sLCAVersion);
    return s;
}


QString &Sec1000SystemInfo::getDeviceName()
{
    return m_sDeviceName;
}


QString &Sec1000SystemInfo::getPCBVersion()
{
    return m_sPCBVersion;
}


QString &Sec1000SystemInfo::getLCAVersion()
{
    return m_sLCAVersion;
}


QString &Sec1000SystemInfo::getSerialNumber()
{
    return m_sSerialNumber;
}







