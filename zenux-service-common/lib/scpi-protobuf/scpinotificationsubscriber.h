#ifndef SCPINOTIFICATIONSUBSCRIBER_H
#define SCPINOTIFICATIONSUBSCRIBER_H

#include <xiqnetpeer.h>
#include <QByteArray>
#include <memory>

class ScpiNotificationSubscriber
{
public:
    ScpiNotificationSubscriber(XiQNetPeer *netPeer, QByteArray clientId, quint16 notifierId);
    ~ScpiNotificationSubscriber();
    XiQNetPeer *m_netPeer;
    QByteArray m_clientId;
    quint16 m_notifierId;
};

typedef std::unique_ptr<ScpiNotificationSubscriber> ScpiNotificationSubscriberPtr;

#endif // SCPINOTIFICATIONSUBSCRIBER_H
