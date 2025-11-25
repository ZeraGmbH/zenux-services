#ifndef MOCKAVAILABLEHOTPLUGDEVICES_H
#define MOCKAVAILABLEHOTPLUGDEVICES_H

#include "abstractmockallservices.h"
#include <QMap>
#include <QString>

class MockAvailableHotplugDevices
{
public:
    static QStringList getAvailableDeviceNames();
    static AbstractMockAllServices::hotplugI2cBus getHotDevices(const QString &hotplugName, const QString &channelAlias);
private:
    static void initMap();
    static QMap<QString, AbstractMockAllServices::hotplugI2cBus> m_availableHotDeviceMap;
};

#endif // MOCKAVAILABLEHOTPLUGDEVICES_H
