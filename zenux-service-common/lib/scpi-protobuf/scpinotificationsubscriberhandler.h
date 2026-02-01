#ifndef SCPINOTIFICATIONSUBSCRIBERHANDLER_H
#define SCPINOTIFICATIONSUBSCRIBERHANDLER_H

#include "scpinotificationsubscriber.h"
#include <QVector>

class ScpiNotificationSubscriberHandler
{
public:
    void addSubscriber(const ScpiNotificationSubscriber &subscriber);
    void removeAllSubscribers(VeinTcp::TcpPeer *netPeer, const QByteArray &clientId);
    int getTotalSubscribers();
    ScpiNotificationSubscriber getSubscriber(int index);
private:
    QVector<ScpiNotificationSubscriber> m_subscriberVector;
};

#endif // SCPINOTIFICATIONSUBSCRIBERHANDLER_H
