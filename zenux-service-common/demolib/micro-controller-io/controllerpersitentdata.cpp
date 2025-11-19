#include "controllerpersitentdata.h"
#include "mt310s2dglobal.h"

ControllerPersitentData::TPersitentControllerData ControllerPersitentData::m_persitentData;

ControllerPersitentData::TPersitentControllerData &ControllerPersitentData::getData()
{
    return m_persitentData;
}

void ControllerPersitentData::injectInterruptFlags(quint16 clampConnectMask)
{
    m_persitentData.m_criticalStatus |= (1<<clampstatusInterrupt);
    m_persitentData.m_clampConnectMask = clampConnectMask;
}

void ControllerPersitentData::setHotplugDevices(const MuxChannelDeviceNameMap &hotDevicesToSet)
{
    m_persitentData.m_hotpluggedDevices = hotDevicesToSet;
}

void ControllerPersitentData::cleanupPersitentData()
{
    m_persitentData.m_permission = false;
    m_persitentData.m_criticalStatus = 0;
    m_persitentData.m_criticalStatusMask = 0;
    m_persitentData.m_clampConnectMask = 0;
    m_persitentData.m_FPGAVersion = "Unknown";
    m_persitentData.m_serialNumber = "Unknown";
    m_persitentData.m_writablePcbVersion = "Unknown";

    m_persitentData.m_pllChannel = 0;

    m_persitentData.m_accuStatus = 0;
    m_persitentData.m_accuCharge = 37;

    m_persitentData.m_hotpluggedDevices.clear();
}
