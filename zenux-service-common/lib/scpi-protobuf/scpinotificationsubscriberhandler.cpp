#include "scpinotificationsubscriberhandler.h"

void ScpiNotificationSubscriberHandler::addSubscriber(ScpiNotificationSubscriber subscriber)
{
    if(!m_subscriberVector.contains(subscriber)){
        m_subscriberVector.push_back(subscriber);
    }
}

void ScpiNotificationSubscriberHandler::removeAllSubscribers(XiQNetPeer *netPeer, QByteArray clientId)
{
    for(int i = m_subscriberVector.size() - 1; i >= 0; i--) {
        if (m_subscriberVector.at(i).m_netPeer == netPeer) {
            if ( clientId.isEmpty() || m_subscriberVector.at(i).m_clientId.isEmpty() || (m_subscriberVector.at(i).m_clientId == clientId)) {
                m_subscriberVector.remove(i);
            }
        }
    }
}

int ScpiNotificationSubscriberHandler::getTotalSubscribers()
{
    return m_subscriberVector.size();
}

ScpiNotificationSubscriber ScpiNotificationSubscriberHandler::getSubscriber(int index)
{
    return m_subscriberVector.at(index);
}
