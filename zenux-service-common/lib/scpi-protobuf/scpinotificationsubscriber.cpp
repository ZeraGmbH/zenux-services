#include "scpinotificationsubscriber.h"

ScpiNotificationSubscriber::ScpiNotificationSubscriber(VeinTcp::TcpPeer *netPeer,
                                                       QByteArray clientId,
                                                       quint16 notifierId,
                                                       bool notifyWithValue) :
    m_netPeer(netPeer),
    m_clientId(clientId),
    m_notifierId(notifierId),
    m_notifyWithValue(notifyWithValue)
{
}

bool operator ==(const ScpiNotificationSubscriber &subscriber1, const ScpiNotificationSubscriber &subscriber2)
{
    return(subscriber1.m_clientId == subscriber2.m_clientId &&
           subscriber1.m_netPeer == subscriber2.m_netPeer &&
           subscriber1.m_notifierId == subscriber2.m_notifierId);
}
