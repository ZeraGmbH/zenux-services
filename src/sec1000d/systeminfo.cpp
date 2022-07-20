#include "systeminfo.h"

cSystemInfo::cSystemInfo()
{
    m_sDeviceName = m_sPCBVersion = m_sLCAVersion = m_sSerialNumber = "Unknown";
    getSystemInfo();
}


void cSystemInfo::getSystemInfo()
{
    m_bRead = true;
}


bool cSystemInfo::dataRead()
{
    return m_bRead;
}


QString cSystemInfo::getDeviceVersion()
{
    QString s = QString ("DEVICE: %1;PCB: %2;LCA: %3")
            .arg(m_sDeviceName)
            .arg(m_sPCBVersion)
            .arg(m_sLCAVersion);
    return s;
}


QString &cSystemInfo::getDeviceName()
{
    return m_sDeviceName;
}


QString &cSystemInfo::getPCBVersion()
{
    return m_sPCBVersion;
}


QString &cSystemInfo::getLCAVersion()
{
    return m_sLCAVersion;
}


QString &cSystemInfo::getSerialNumber()
{
    return m_sSerialNumber;
}







