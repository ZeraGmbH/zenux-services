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

private:
    ScpiNotificationSubscriberHandler *m_notificationHandler;
    std::unique_ptr<ScpiNotificationSubscriber> m_subscriber;
};

#endif // TEST_NOTIFICATIONSUBSCRIBER_H
