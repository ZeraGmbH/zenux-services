#ifndef NOTIFICATIONDATA_H
#define NOTIFICATIONDATA_H

#include <QByteArray>
#include <xiqnetpeer.h>

#include <notificationstring.h>

struct cNotificationData
{
    XiQNetPeer *netPeer;
    QByteArray clientID;
    quint16 notifier;
    cNotificationString *notString;
};

#endif // NOTIFICATIONDATA_H
