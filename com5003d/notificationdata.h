#ifndef NOTIFICATIONDATA_H
#define NOTIFICATIONDATA_H

#include <QByteArray>
#include <xiqnetpeer.h>

#include "notificationstring.h"

struct cNotificationData
{
    XiQNetPeer *netPeer;
    QByteArray clientId;
    quint16 notifierId;
    NotificationString *notString;
};

#endif // NOTIFICATIONDATA_H
