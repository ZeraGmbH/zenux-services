#ifndef SCPINOTIFICATIONSUBSCRIBERHANDLER_H
#define SCPINOTIFICATIONSUBSCRIBERHANDLER_H

#include "scpinotificationsubscriber.h"
#include <QVector>

class ScpiNotificationSubscriberHandler
{
public:
    bool addSubscriber(ScpiNotificationSubscriber subscriber);
    bool removeSubscriber(ScpiNotificationSubscriber subscriber);
    int getTotalSubscribers();
private:
    QVector<ScpiNotificationSubscriber> m_subscriberVector;
};

#endif // SCPINOTIFICATIONSUBSCRIBERHANDLER_H
