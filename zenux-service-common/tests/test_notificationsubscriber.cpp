#include "test_notificationsubscriber.h"
#include <QTest>
#include <memory>

QTEST_MAIN(test_notificationsubscriber);

void test_notificationsubscriber::init()
{
    m_netPeer = new XiQNetPeer();
    m_notificationHandler = new ScpiNotificationSubscriberHandler();
}

void test_notificationsubscriber::cleanup()
{
    delete m_notificationHandler;
    delete m_netPeer;
}

void test_notificationsubscriber::addAndRemoveSubscriber()
{
    ScpiNotificationSubscriber subscriberA(nullptr, "", 1);
    m_notificationHandler->addSubscriber(subscriberA);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 1);
    m_notificationHandler->removeAllSubscribers(nullptr, "");
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 0);
}

void test_notificationsubscriber::addSubscriberTwice()
{
    ScpiNotificationSubscriber subscriberA(nullptr, "", 1);
    m_notificationHandler->addSubscriber(subscriberA);
    m_notificationHandler->addSubscriber(subscriberA);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 1);
}

void test_notificationsubscriber::addTwoSubscribersDifferentNetPeer()
{
    ScpiNotificationSubscriber subscriber1(nullptr, "", 1);
    ScpiNotificationSubscriber subscriber2(m_netPeer, QByteArray(), 2);
    m_notificationHandler->addSubscriber(subscriber1);
    m_notificationHandler->addSubscriber(subscriber2);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 2);
}

void test_notificationsubscriber::addTwoSubscribersSameNetPeer()
{
    ScpiNotificationSubscriber subscriber1(m_netPeer, QByteArray(), 1);
    ScpiNotificationSubscriber subscriber2(m_netPeer, QByteArray(), 2);
    m_notificationHandler->addSubscriber(subscriber1);
    m_notificationHandler->addSubscriber(subscriber2);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 2);
}

void test_notificationsubscriber::addAndRemoveSubscribersSamePeerSameClientID()
{
    ScpiNotificationSubscriber subscriber1(m_netPeer, "Foo", 1);
    ScpiNotificationSubscriber subscriber2(m_netPeer, "Foo", 2);
    m_notificationHandler->addSubscriber(subscriber1);
    m_notificationHandler->addSubscriber(subscriber2);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 2);
    m_notificationHandler->removeAllSubscribers(m_netPeer, "Foo");
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 0);
}

void test_notificationsubscriber::addAndRemoveSubscribersSamePeerDifferentClientID()
{
    ScpiNotificationSubscriber subscriber1(m_netPeer, "Foo", 1);
    ScpiNotificationSubscriber subscriber2(m_netPeer, "Bar", 2);
    m_notificationHandler->addSubscriber(subscriber1);
    m_notificationHandler->addSubscriber(subscriber2);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 2);
    m_notificationHandler->removeAllSubscribers(m_netPeer, "Foo");
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 1);
    m_notificationHandler->removeAllSubscribers(m_netPeer, "Bar");
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 0);
}

void test_notificationsubscriber::addAndRemoveSubscribersSamePeerEmptyClientID()
{
    ScpiNotificationSubscriber subscriber1(m_netPeer, "", 1);
    ScpiNotificationSubscriber subscriber2(m_netPeer, "Foo", 2);
    m_notificationHandler->addSubscriber(subscriber1);
    m_notificationHandler->addSubscriber(subscriber2);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 2);
    m_notificationHandler->removeAllSubscribers(m_netPeer, "Foo");
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 0);
}

void test_notificationsubscriber::removeSubscribersEmptyClientIDPassed()
{
    ScpiNotificationSubscriber subscriber2(m_netPeer, "Foo", 1);
    ScpiNotificationSubscriber subscriber1(m_netPeer, "Bar", 1);
    ScpiNotificationSubscriber subscriber3(m_netPeer, "", 1);
    m_notificationHandler->addSubscriber(subscriber1);
    m_notificationHandler->addSubscriber(subscriber2);
    m_notificationHandler->addSubscriber(subscriber3);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 3);
    m_notificationHandler->removeAllSubscribers(m_netPeer, "");
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 0);
}
