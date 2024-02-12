#ifndef NOTIFICATIONSTRUCTWITHVALUE_H
#define NOTIFICATIONSTRUCTWITHVALUE_H

#include "notificationvalue.h"
#include <vtcp_peer.h>
#include <QByteArray>

struct NotificationStructWithValue
{
    VeinTcp::TcpPeer *netPeer;
    QByteArray clientID;
    NotificationValue *notValue;
};

#endif // NOTIFICATIONSTRUCTWITHVALUE_H
