#include "systeminfo.h"

SystemInfo::SystemInfo(AbstractFactoryI2cCtrlPtr ctrlFactory) :
    m_ctrlFactory(ctrlFactory)
{
    m_sDeviceName = m_sPCBVersion = m_sLCAVersion = m_sCTRLVersion = m_sSerialNumber = "Unknown";
    getSystemInfo();
}

void SystemInfo::getSystemInfo()
{
    I2cCtrlCommonInfoPtrUnique controller = m_ctrlFactory->getCommonInfoController(AbstractFactoryI2cCtrl::CTRL_TYPE_RELAIS);
    I2cCtrlDeviceIdentPtr idController = m_ctrlFactory->getDeviceIdentController();
    int rm = ZeraMControllerIo::cmddone;
    rm |= idController->readDeviceName(m_sDeviceName);
    rm |= controller->readPCBInfo(m_sPCBVersion);
    rm |= idController->readLCAVersion(m_sLCAVersion);
    rm |= controller->readCTRLVersion(m_sCTRLVersion);
    rm |= idController->readSerialNumber(m_sSerialNumber);
    m_bRead = (rm == ZeraMControllerIo::cmddone);
}

bool SystemInfo::dataRead()
{
    return m_bRead;
}

QString SystemInfo::getDeviceVersion()
{
    return QString("DEVICE: %1;PCB: %2;LCA: %3;CTRL: %4").arg(
        m_sDeviceName,
        m_sPCBVersion,
        m_sLCAVersion,
        m_sCTRLVersion);
}

QString SystemInfo::getDeviceName()
{
    return m_sDeviceName;
}

QString SystemInfo::getPCBVersion()
{
    return m_sPCBVersion;
}

QString SystemInfo::getLCAVersion()
{
    return m_sLCAVersion;
}

QString SystemInfo::getCTRLVersion()
{
    return m_sCTRLVersion;
}

QString SystemInfo::getSerialNumber()
{
    return m_sSerialNumber;
}
