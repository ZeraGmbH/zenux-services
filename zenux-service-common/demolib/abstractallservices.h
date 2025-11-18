#ifndef ABSTRACTALLSERVICES_H
#define ABSTRACTALLSERVICES_H

#include <QStringList>

class AbstractAllServices
{
public:
    virtual ~AbstractAllServices() = default;
    virtual void fireHotplugInterrupt(const QStringList &channelAliases) = 0;
    typedef QMap<QString, QString> ChannelAliasHotplugDeviceNameMap;
    virtual void fireHotplugInterruptControllerName(const ChannelAliasHotplugDeviceNameMap &infoMap) = 0;
};

#endif // ABSTRACTALLSERVICES_H
