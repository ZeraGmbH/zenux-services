#ifndef NOTIFICATIONDATA_H
#define NOTIFICATIONDATA_H

#include "notificationvalue.h"
#include <xiqnetpeer.h>
#include <QByteArray>

struct NotificationStructWithValue
{
    XiQNetPeer *netPeer;
    QByteArray clientID;
    NotificationValue *notValue;
};

#endif // NOTIFICATIONDATA_H
