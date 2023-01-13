#ifndef NOTIFICATIONDATA_H
#define NOTIFICATIONDATA_H

#include <QByteArray>
#include <xiqnetpeer.h>

#include <notificationvalue.h>

struct cNotificationData
{
    XiQNetPeer *netPeer;
    QByteArray clientID;
    NotificationValue *notValue;
};

#endif // NOTIFICATIONDATA_H
