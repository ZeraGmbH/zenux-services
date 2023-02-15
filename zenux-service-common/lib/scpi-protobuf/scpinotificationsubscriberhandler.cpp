#include "scpinotificationsubscriberhandler.h"

bool ScpiNotificationSubscriberHandler::addSubscriber(const ScpiNotificationSubscriber &subscriber)
{
    bool ok = false;
    if(!m_subscriberVector.contains(subscriber)){
        m_subscriberVector.push_back(subscriber);
        ok = true;
    }
    return ok;
}

bool ScpiNotificationSubscriberHandler::removeSubscriber(const ScpiNotificationSubscriber &subscriber)
{
    bool ok = false;
    if(m_subscriberVector.contains(subscriber)){
        m_subscriberVector.removeOne(subscriber);
        ok = true;
    }
    return ok;
}

int ScpiNotificationSubscriberHandler::getTotalSubscribers()
{
    return m_subscriberVector.size();
}
