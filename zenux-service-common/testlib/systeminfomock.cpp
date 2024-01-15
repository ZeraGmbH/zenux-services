#include "systeminfomock.h"

SystemInfoMock::SystemInfoMock(FactoryControllerAbstractPtr ctrlFactory) :
    SystemInfo(ctrlFactory)
{
}

QString SystemInfoMock::getDeviceName()
{
    return m_deviceName;
}

QString SystemInfoMock::getPCBVersion()
{
    return m_pcpVersion;
}

QString SystemInfoMock::getLCAVersion()
{
    return m_lcaVersion;
}

QString SystemInfoMock::getCTRLVersion()
{
    return m_ctrlVersion;
}

QString SystemInfoMock::getSerialNumber()
{
    return m_serialNumber;
}

void SystemInfoMock::setDeviceName(QString name)
{
    m_deviceName = name;
}

void SystemInfoMock::setPCBVersion(QString version)
{
    m_pcpVersion = version;
}

void SystemInfoMock::setLCAVersion(QString version)
{
    m_lcaVersion = version;
}

void SystemInfoMock::setCTRLVersion(QString version)
{
    m_ctrlVersion = version;
}

void SystemInfoMock::setSerialNumber(QString serno)
{
    m_serialNumber = serno;
}
