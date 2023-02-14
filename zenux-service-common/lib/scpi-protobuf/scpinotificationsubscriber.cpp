#include "scpinotificationsubscriber.h"

ScpiNotificationSubscriber::ScpiNotificationSubscriber(XiQNetPeer *netPeer, QByteArray clientId, quint16 notifierId) :
    m_netPeer(netPeer),
    m_clientId(clientId),
    m_notifierId(notifierId)
{
}

ScpiNotificationSubscriber::~ScpiNotificationSubscriber()
{
    if(m_netPeer) delete m_netPeer;
}
