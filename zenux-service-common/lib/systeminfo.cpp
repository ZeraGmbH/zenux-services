#include "systeminfo.h"
#include "micro-controller-io/atmel.h"

cSystemInfo::cSystemInfo()
{
    m_sDeviceName = m_sPCBVersion = m_sLCAVersion = m_sCTRLVersion = m_sSerialNumber = "Unknown";
    getSystemInfo();
}


void cSystemInfo::getSystemInfo()
{
    int rm = ZeraMControllerIo::cmddone;
    rm |= Atmel::getInstance().readDeviceName(m_sDeviceName);
    rm |= Atmel::getInstance().readPCBVersion(m_sPCBVersion);
    rm |= Atmel::getInstance().readLCAVersion(m_sLCAVersion);
    rm |= Atmel::getInstance().readCTRLVersion(m_sCTRLVersion);
    rm |= Atmel::getInstance().readSerialNumber(m_sSerialNumber);
    m_bRead = (rm == ZeraMControllerIo::cmddone);
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


QString &cSystemInfo::getCTRLVersion()
{
    return m_sCTRLVersion;
}


QString &cSystemInfo::getSerialNumber()
{
    return m_sSerialNumber;
}







