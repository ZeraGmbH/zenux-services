#ifndef ABSTRACTMOCKALLSERVICES_H
#define ABSTRACTMOCKALLSERVICES_H

#include <QStringList>
#include "clamp.h"

class AbstractMockAllServices
{
public:
    virtual ~AbstractMockAllServices() = default;
    virtual void fireHotplugInterruptControllerOnly(const QStringList &channelAliases) = 0;
    struct hotplugI2cBus
    {
        QString controllerName;
        cClamp::ClampTypes clamp;
    };
    typedef QMap<QString, hotplugI2cBus> ChannelAliasHotplugDeviceNameMap;
    virtual void fireHotplugInterrupt(const ChannelAliasHotplugDeviceNameMap &infoMap) = 0;
};

#endif // ABSTRACTMOCKALLSERVICES_H
