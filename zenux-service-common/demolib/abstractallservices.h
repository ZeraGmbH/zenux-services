#ifndef ABSTRACTALLSERVICES_H
#define ABSTRACTALLSERVICES_H

#include <QStringList>

class AbstractAllServices
{
public:
    virtual ~AbstractAllServices() = default;
    virtual void fireHotplugInterrupt(const QStringList &channelAliases) = 0;
    virtual void fireHotplugInterruptControllerName(const QMap<QString,QString> &infoMap) = 0;
};

#endif // ABSTRACTALLSERVICES_H
