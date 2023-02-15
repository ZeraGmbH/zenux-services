#ifndef TEST_NOTIFICATIONSUBSCRIBER_H
#define TEST_NOTIFICATIONSUBSCRIBER_H

#include "scpinotificationsubscriberhandler.h"
#include <QObject>
#include <memory>

class test_notificationsubscriber: public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void addAndRemoveSubscriber();
    void removeNonAddedSubscriber();
    void addSubscriberTwice();

    void addTwoSubscribersDifferentNetPeer();
    void addTwoSubscribersSameNetPeer();
    void addAndRemoveSubscribersFromSamePeer();

private:
    ScpiNotificationSubscriberHandler *m_notificationHandler;
    std::unique_ptr<ScpiNotificationSubscriber> m_subscriberA;
    XiQNetPeer* m_netPeer;
};

#endif // TEST_NOTIFICATIONSUBSCRIBER_H
