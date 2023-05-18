#ifndef TEST_NOTIFICATIONSUBSCRIBER_H
#define TEST_NOTIFICATIONSUBSCRIBER_H

#include "scpinotificationsubscriberhandler.h"
#include <memory>

class test_notificationsubscriber: public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void addAndRemoveSubscriber();
    void addSubscriberTwice();

    void addTwoSubscribersDifferentNetPeer();
    void addTwoSubscribersSameNetPeer();

    void addAndRemoveSubscribersSamePeerSameClientID();
    void addAndRemoveSubscribersSamePeerDifferentClientID();
    void addAndRemoveSubscribersSamePeerEmptyClientID();
    void removeSubscribersEmptyClientIDPassed();

private:
    ScpiNotificationSubscriberHandler *m_notificationHandler;
    XiQNetPeer* m_netPeer;
};

#endif // TEST_NOTIFICATIONSUBSCRIBER_H
