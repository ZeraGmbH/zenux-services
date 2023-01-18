#ifndef NOTIFICATIONSTRUCTWITHVALUE_H
#define NOTIFICATIONSTRUCTWITHVALUE_H

#include "notificationvalue.h"
#include <xiqnetpeer.h>
#include <QByteArray>

struct NotificationStructWithValue
{
    XiQNetPeer *netPeer;
    QByteArray clientID;
    NotificationValue *notValue;
};

#endif // NOTIFICATIONSTRUCTWITHVALUE_H
