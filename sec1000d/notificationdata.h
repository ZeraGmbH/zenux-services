#ifndef NOTIFICATIONDATA_H
#define NOTIFICATIONDATA_H

#include "notificationvalue.h"
#include <QByteArray>
#include <xiqnetpeer.h>

struct cNotificationData
{
    XiQNetPeer *netPeer;
    QByteArray clientID;
    NotificationValue *notValue;
};

#endif // NOTIFICATIONDATA_H
