#include "testsysteminfo.h"

TestSystemInfo::TestSystemInfo(FactoryControllerAbstractPtr ctrlFactory) :
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
    return m_lcaVersion;
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
    m_lcaVersion = version;
}

void TestSystemInfo::setCTRLVersion(QString version)
{
    m_ctrlVersion = version;
}

void TestSystemInfo::setSerialNumber(QString serno)
{
    m_serialNumber = serno;
}
