#ifndef NOTIFICATIONSTRUCTWITHSTRINGANDID_H
#define NOTIFICATIONSTRUCTWITHSTRINGANDID_H

#include "notificationstring.h"
#include <vtcp_peer.h>
#include <QByteArray>

struct NotificationStructWithStringAndId
{
    VeinTcp::TcpPeer *netPeer;
    QByteArray clientId;
    quint16 notifierId;
    NotificationString *notString;
};

#endif // NOTIFICATIONSTRUCTWITHSTRINGANDID_H
