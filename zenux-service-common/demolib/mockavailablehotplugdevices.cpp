#include "mockavailablehotplugdevices.h"
#include "clamp.h"

QMap<QString, AbstractMockAllServices::hotplugI2cBus> MockAvailableHotplugDevices::m_availableHotDeviceMap;

QStringList MockAvailableHotplugDevices::getAvailableDeviceNames()
{
    initMap();
    return m_availableHotDeviceMap.keys();
}

AbstractMockAllServices::hotplugI2cBus MockAvailableHotplugDevices::getHotDevices(const QString &hotplugName, const QString &channelAlias)
{
    initMap();
    AbstractMockAllServices::hotplugI2cBus device = m_availableHotDeviceMap.value(hotplugName);

    // We can adjust what was build more - for now a simple guess...
    if (channelAlias != "IAUX" && device.controllerName != "MT650e")
        device.controllerName = "";

    return device;
}

void MockAvailableHotplugDevices::initMap()
{
    if(!m_availableHotDeviceMap.isEmpty())
        return;

    m_availableHotDeviceMap["--"] = AbstractMockAllServices::hotplugI2cBus();
    for (int clampType=cClamp::undefined+1; clampType<cClamp::anzCL; ++clampType) {
        const QString deviceName = cClamp::getClampTypeName(clampType);
        if (deviceName.startsWith("EMOB", Qt::CaseInsensitive))
            m_availableHotDeviceMap[deviceName] = AbstractMockAllServices::hotplugI2cBus{deviceName, cClamp::ClampTypes(clampType)};
        else
            m_availableHotDeviceMap[deviceName] = AbstractMockAllServices::hotplugI2cBus{"", cClamp::ClampTypes(clampType)};
    }
    m_availableHotDeviceMap["MT650e"] = AbstractMockAllServices::hotplugI2cBus{"MT650e", cClamp::undefined};
}
