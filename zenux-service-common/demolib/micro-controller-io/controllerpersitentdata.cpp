#include "controllerpersitentdata.h"
#include "mt310s2dglobal.h"

std::unique_ptr<ControllerPersitentData::TPersitentControllerData> ControllerPersitentData::m_persitentData;

ControllerPersitentData::TPersitentControllerData &ControllerPersitentData::getData()
{
    if (m_persitentData == nullptr)
        m_persitentData = std::make_unique<TPersitentControllerData>();
    return *m_persitentData;
}

void ControllerPersitentData::injectInterruptFlags(quint16 clampConnectMask)
{
    m_persitentData->m_criticalStatus |= (1<<clampstatusInterrupt);
    m_persitentData->m_clampConnectMask = clampConnectMask;
}

void ControllerPersitentData::setHotplugDevices(const MuxChannelDeviceNameMap &hotDevicesToSet)
{
    m_persitentData->m_hotpluggedDevices = hotDevicesToSet;
}

bool ControllerPersitentData::isHotControllerAvailable(qint8 muxChannel)
{
    return m_persitentData->m_hotpluggedDevices.contains(muxChannel) &&
           !m_persitentData->m_hotpluggedDevices[muxChannel].controllerName.isEmpty();
}

bool ControllerPersitentData::isControllerAvailable(qint8 muxChannel)
{
    return muxChannel < 0 || isHotControllerAvailable(muxChannel);
}

void ControllerPersitentData::cleanupPersitentData()
{
    m_persitentData.reset();
}
