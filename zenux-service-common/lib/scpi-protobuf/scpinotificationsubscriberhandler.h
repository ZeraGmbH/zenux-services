#ifndef SCPINOTIFICATIONSUBSCRIBERHANDLER_H
#define SCPINOTIFICATIONSUBSCRIBERHANDLER_H

#include "scpinotificationsubscriber.h"
#include <QVector>

class ScpiNotificationSubscriberHandler
{
public:
    ScpiNotificationSubscriberHandler();
    bool addSubscriber(const ScpiNotificationSubscriber &subscriber);
    bool removeSubscriber(const ScpiNotificationSubscriber &subscriber);
    int getTotalSubscribers();
private:
    QVector<ScpiNotificationSubscriber> m_subscriberVector;
};

#endif // SCPINOTIFICATIONSUBSCRIBERHANDLER_H
