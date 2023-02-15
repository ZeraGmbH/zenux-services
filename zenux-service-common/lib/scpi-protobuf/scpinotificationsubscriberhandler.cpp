#include "scpinotificationsubscriberhandler.h"

bool ScpiNotificationSubscriberHandler::addSubscriber(ScpiNotificationSubscriber subscriber)
{
    bool ok = false;
    if(!m_subscriberVector.contains(subscriber)){
        m_subscriberVector.push_back(subscriber);
        ok = true;
    }
    return ok;
}

bool ScpiNotificationSubscriberHandler::removeSubscriber(ScpiNotificationSubscriber subscriber)
{
    bool ok = false;
    if(m_subscriberVector.contains(subscriber)){
        m_subscriberVector.removeOne(subscriber);
        ok = true;
    }
    return ok;
}

void ScpiNotificationSubscriberHandler::removeAllSubscribersFromAPeer(XiQNetPeer *netPeer)
{
    for(auto subscriber : qAsConst(m_subscriberVector)) {
        if (subscriber.getXiQNetPeer() == netPeer) {
            m_subscriberVector.removeOne(subscriber);
        }
    }
}

int ScpiNotificationSubscriberHandler::getTotalSubscribers()
{
    return m_subscriberVector.size();
}
