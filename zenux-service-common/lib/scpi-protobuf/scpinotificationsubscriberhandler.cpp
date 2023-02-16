#include "scpinotificationsubscriberhandler.h"

void ScpiNotificationSubscriberHandler::addSubscriber(ScpiNotificationSubscriber subscriber)
{
    if(!m_subscriberVector.contains(subscriber)){
        m_subscriberVector.push_back(subscriber);
    }
}

void ScpiNotificationSubscriberHandler::removeAllSubscribersFromAPeer(XiQNetPeer *netPeer)
{
    for(auto subscriber : qAsConst(m_subscriberVector)) {
        if (subscriber.m_netPeer == netPeer) {
            m_subscriberVector.removeOne(subscriber);
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
