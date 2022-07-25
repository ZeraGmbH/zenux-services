#include "systeminfo.h"
#include "micro-controller-io/atmelsysctrl.h"
#include "micro-controller-io/atmel.h"

cSystemInfo::cSystemInfo()
{
    m_sDeviceName = m_sPCBVersion = m_sLCAVersion = m_sSysCTRLVersion = m_sCTRLVersion = m_sSerialNumber = "Unknown";
    getSystemInfo();
}


void cSystemInfo::getSystemInfo()
{
    int rm = ZeraMcontrollerBase::cmddone;
    rm |= pAtmel->readDeviceName(m_sDeviceName);
    rm |= pAtmel->readPCBVersion(m_sPCBVersion);
    rm |= pAtmel->readLCAVersion(m_sLCAVersion);
    rm |= pAtmelSys->readCTRLVersion(m_sSysCTRLVersion);
    rm |= pAtmel->readCTRLVersion(m_sCTRLVersion);
    rm |= pAtmel->readSerialNumber(m_sSerialNumber);
    m_bRead = (rm == ZeraMcontrollerBase::cmddone);
}


bool cSystemInfo::dataRead()
{
    return m_bRead;
}


QString cSystemInfo::getDeviceVersion()
{
    QString s = QString ("DEVICE: %1;PCB: %2;LCA: %3;CTRL: %4")
            .arg(m_sDeviceName)
            .arg(m_sPCBVersion)
            .arg(m_sLCAVersion)
            .arg(m_sCTRLVersion);
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


QString &cSystemInfo::getSysCTRLVersion()
{
    return m_sSysCTRLVersion;
}


QString &cSystemInfo::getCTRLVersion()
{
    return m_sCTRLVersion;
}


QString &cSystemInfo::getSerialNumber()
{
    return m_sSerialNumber;
}







