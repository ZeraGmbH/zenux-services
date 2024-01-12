#include "mt310s2systeminfomock.h"

Mt310s2SystemInfoMock::Mt310s2SystemInfoMock(AtmelCtrlFactoryInterfacePrt ctrlFactory) :
    Mt310s2SystemInfo(ctrlFactory)
{
}

QString Mt310s2SystemInfoMock::getDeviceName()
{
    return m_deviceName;
}

QString Mt310s2SystemInfoMock::getPCBVersion()
{
    return m_pcpVersion;
}

QString Mt310s2SystemInfoMock::getLCAVersion()
{
    return m_lcaVersion;
}

QString Mt310s2SystemInfoMock::getCTRLVersion()
{
    return m_ctrlVersion;
}

QString Mt310s2SystemInfoMock::getSerialNumber()
{
    return m_serialNumber;
}

void Mt310s2SystemInfoMock::setDeviceName(QString name)
{
    m_deviceName = name;
}

void Mt310s2SystemInfoMock::setPCBVersion(QString version)
{
    m_pcpVersion = version;
}

void Mt310s2SystemInfoMock::setLCAVersion(QString version)
{
    m_lcaVersion = version;
}

void Mt310s2SystemInfoMock::setCTRLVersion(QString version)
{
    m_ctrlVersion = version;
}

void Mt310s2SystemInfoMock::setSerialNumber(QString serno)
{
    m_serialNumber = serno;
}

QString Mt310s2SystemInfoMock::getSysCTRLVersion()
{
    return "Unknown";
}

QString Mt310s2SystemInfoMock::getSysPCBVersion()
{
    return "Unknown";
}
