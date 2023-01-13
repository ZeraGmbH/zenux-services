#ifndef NOTIFICATIONDATA_H
#define NOTIFICATIONDATA_H

#include "notificationstring.h"
#include <xiqnetpeer.h>
#include <QByteArray>

struct cNotificationData
{
    XiQNetPeer *netPeer;
    QByteArray clientID;
    quint16 notifier;
    NotificationString *notString;
};

#endif // NOTIFICATIONDATA_H
