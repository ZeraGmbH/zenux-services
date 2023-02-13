#ifndef SCPINOTIFICATIONSUBSCRIBER_H
#define SCPINOTIFICATIONSUBSCRIBER_H

#include <xiqnetpeer.h>
#include <QByteArray>
#include <memory>

struct ScpiNotificationSubscriber
{
    XiQNetPeer *netPeer;
    QByteArray clientId;
    quint16 notifierId;
};

typedef std::unique_ptr<ScpiNotificationSubscriber> ScpiNotificationSubscriberPtr;

#endif // SCPINOTIFICATIONSUBSCRIBER_H
