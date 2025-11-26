#ifndef ABSTRACTMOCKALLSERVICES_H
#define ABSTRACTMOCKALLSERVICES_H

#include <QStringList>
#include "clamp.h"

class AbstractMockAllServices
{
public:
    virtual ~AbstractMockAllServices() = default;

    // It's all about hotplug:
    // Full functional methods
    struct hotplugI2cBus {
        QString controllerName;
        cClamp::ClampTypes clamp = cClamp::undefined;
    };
    typedef QMap<QString, hotplugI2cBus> ChannelAliasHotplugDeviceNameMap;
    virtual void fireHotplugInterrupt(const ChannelAliasHotplugDeviceNameMap &deviceMap) = 0;
    virtual ChannelAliasHotplugDeviceNameMap getCurrentHotplugMap() const = 0;

    // Convenience methods
    virtual void addStandardEmobControllers(const QStringList &channelAliases) = 0;
    struct clampParam {
        QString channelAlias;
        cClamp::ClampTypes clamp = cClamp::undefined;
    };
    virtual void removeAllHotplugDevices() = 0;
    virtual void addClamps(const QList<clampParam> &clampParams) = 0;
};

#endif // ABSTRACTMOCKALLSERVICES_H
