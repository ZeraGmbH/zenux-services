#ifndef NOTIFICATIONSTRUCTWITHSTRINGANDID_H
#define NOTIFICATIONSTRUCTWITHSTRINGANDID_H

#include "notificationstring.h"
#include <xiqnetpeer.h>
#include <QByteArray>

struct NotificationStructWithStringAndId
{
    XiQNetPeer *netPeer;
    QByteArray clientId;
    quint16 notifierId;
    NotificationString *notString;
};

#endif // NOTIFICATIONSTRUCTWITHSTRINGANDID_H
