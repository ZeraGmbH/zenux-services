#ifndef SCPINOTIFICATIONSUBSCRIBERHANDLER_H
#define SCPINOTIFICATIONSUBSCRIBERHANDLER_H

#include "scpinotificationsubscriber.h"
#include <QVector>

class ScpiNotificationSubscriberHandler
{
public:
    void addSubscriber(ScpiNotificationSubscriber subscriber);
    void removeAllSubscribers(XiQNetPeer *netPeer, QByteArray clientId);
    int getTotalSubscribers();
    ScpiNotificationSubscriber getSubscriber(int index);
private:
    QVector<ScpiNotificationSubscriber> m_subscriberVector;
};

#endif // SCPINOTIFICATIONSUBSCRIBERHANDLER_H
