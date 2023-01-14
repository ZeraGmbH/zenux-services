#ifndef NOTIFICATIONDATA_H
#define NOTIFICATIONDATA_H

#include "notificationstring.h"
#include <xiqnetpeer.h>
#include <QByteArray>

struct NotificationStructForPcb
{
    XiQNetPeer *netPeer;
    QByteArray clientId;
    quint16 notifierId;
    NotificationString *notString;
};

#endif // NOTIFICATIONDATA_H
