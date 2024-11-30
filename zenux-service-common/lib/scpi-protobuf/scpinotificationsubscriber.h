#ifndef SCPINOTIFICATIONSUBSCRIBER_H
#define SCPINOTIFICATIONSUBSCRIBER_H

#include <vtcp_peer.h>
#include <QByteArray>

class ScpiNotificationSubscriber
{
    friend bool operator == (const ScpiNotificationSubscriber &subscriber1, const ScpiNotificationSubscriber &subscriber2);
public:
    ScpiNotificationSubscriber(VeinTcp::TcpPeer *netPeer,
                               QByteArray clientId,
                               quint16 notifierId,
                               bool notifyWithValue);
    VeinTcp::TcpPeer *m_netPeer;
    QByteArray m_clientId;
    quint16 m_notifierId;
    bool m_notifyWithValue;
};

#endif // SCPINOTIFICATIONSUBSCRIBER_H
