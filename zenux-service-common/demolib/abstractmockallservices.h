#ifndef ABSTRACTMOCKALLSERVICES_H
#define ABSTRACTMOCKALLSERVICES_H

#include <QStringList>

class AbstractMockAllServices
{
public:
    virtual ~AbstractMockAllServices() = default;
    virtual void fireHotplugInterrupt(const QStringList &channelAliases) = 0;
    typedef QMap<QString, QString> ChannelAliasHotplugDeviceNameMap;
    virtual void fireHotplugInterruptControllerName(const ChannelAliasHotplugDeviceNameMap &infoMap) = 0;
};

#endif // ABSTRACTMOCKALLSERVICES_H
