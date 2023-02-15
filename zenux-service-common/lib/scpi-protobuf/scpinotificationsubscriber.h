#ifndef SCPINOTIFICATIONSUBSCRIBER_H
#define SCPINOTIFICATIONSUBSCRIBER_H

#include <xiqnetpeer.h>
#include <QByteArray>
#include <memory>

class ScpiNotificationSubscriber
{
    friend bool operator == (const ScpiNotificationSubscriber &subscriber1, const ScpiNotificationSubscriber &subscriber2);
public:
    ScpiNotificationSubscriber(XiQNetPeer *netPeer, QByteArray clientId, quint16 notifierId);
    XiQNetPeer *m_netPeer;
    QByteArray m_clientId;
    quint16 m_notifierId;
};

#endif // SCPINOTIFICATIONSUBSCRIBER_H
