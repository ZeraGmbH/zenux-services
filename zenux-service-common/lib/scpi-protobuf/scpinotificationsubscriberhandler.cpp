#include "scpinotificationsubscriberhandler.h"

void ScpiNotificationSubscriberHandler::addSubscriber(ScpiNotificationSubscriber subscriber)
{
    if(!m_subscriberVector.contains(subscriber)){
        m_subscriberVector.push_back(subscriber);
        qInfo("Subscriber added: Peer %i, clientId %s, NotifierID %i, Total %i",
              subscriber.m_netPeer, qPrintable(subscriber.m_clientId), subscriber.m_notifierId, m_subscriberVector.size());
    }
}

void ScpiNotificationSubscriberHandler::removeAllSubscribers(XiQNetPeer *netPeer, QByteArray clientId)
{
    if (m_subscriberVector.size()) {
        qInfo("Total subscribers %i", m_subscriberVector.size());
    }
    for(int i = m_subscriberVector.size() - 1; i >= 0; i--) {
        if (m_subscriberVector.at(i).m_netPeer == netPeer) {
            if ( clientId.isEmpty() || m_subscriberVector.at(i).m_clientId.isEmpty() || (m_subscriberVector.at(i).m_clientId == clientId)) {
                m_subscriberVector.remove(i);
                qInfo("Subscriber removed : Peer %i, clientId %s, total subscriber %i", netPeer, qPrintable(clientId), m_subscriberVector.size());
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
