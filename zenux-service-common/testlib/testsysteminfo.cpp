#include "testsysteminfo.h"

TestSystemInfo::TestSystemInfo(AbstractFactoryI2cCtrlPtr ctrlFactory) :
    SystemInfo(ctrlFactory)
{
}

QString TestSystemInfo::getDeviceName()
{
    return m_deviceName;
}

QString TestSystemInfo::getPCBVersion()
{
    return m_pcpVersion;
}

QString TestSystemInfo::getLCAVersion()
{
    return m_FPGAVersion;
}

QString TestSystemInfo::getCTRLVersion()
{
    return m_ctrlVersion;
}

QString TestSystemInfo::getSerialNumber()
{
    return m_serialNumber;
}

void TestSystemInfo::setDeviceName(QString name)
{
    m_deviceName = name;
}

void TestSystemInfo::setPCBVersion(QString version)
{
    m_pcpVersion = version;
}

void TestSystemInfo::setLCAVersion(QString version)
{
    m_FPGAVersion = version;
}

void TestSystemInfo::setCTRLVersion(QString version)
{
    m_ctrlVersion = version;
}

void TestSystemInfo::setSerialNumber(QString serno)
{
    m_serialNumber = serno;
}
