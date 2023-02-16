#include "test_notificationsubscriber.h"
#include <QTest>
#include <memory>

QTEST_MAIN(test_notificationsubscriber);

void test_notificationsubscriber::init()
{
    m_netPeer = new XiQNetPeer();
    m_subscriberA = std::make_unique<ScpiNotificationSubscriber>(nullptr,QByteArray(),1);
    m_notificationHandler = new ScpiNotificationSubscriberHandler();
}

void test_notificationsubscriber::cleanup()
{
    delete m_notificationHandler;
    delete m_netPeer;
}

void test_notificationsubscriber::addAndRemoveSubscriber()
{
    m_notificationHandler->addSubscriber(*m_subscriberA);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 1);
    m_notificationHandler->removeAllSubscribersFromAPeer(nullptr);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 0);
}

void test_notificationsubscriber::addSubscriberTwice()
{
    m_notificationHandler->addSubscriber(*m_subscriberA);
    m_notificationHandler->addSubscriber(*m_subscriberA);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 1);
}

void test_notificationsubscriber::addTwoSubscribersDifferentNetPeer()
{
    ScpiNotificationSubscriber subscriberB(m_netPeer,QByteArray(),2);
    m_notificationHandler->addSubscriber(*m_subscriberA);
    m_notificationHandler->addSubscriber(subscriberB);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 2);
}

void test_notificationsubscriber::addTwoSubscribersSameNetPeer()
{
    ScpiNotificationSubscriber subscriber1(m_netPeer,QByteArray(),1);
    ScpiNotificationSubscriber subscriber2(m_netPeer,QByteArray(),2);
    m_notificationHandler->addSubscriber(subscriber1);
    m_notificationHandler->addSubscriber(subscriber2);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 2);
}

void test_notificationsubscriber::addAndRemoveSubscribersFromSamePeer()
{
    ScpiNotificationSubscriber subscriber1(m_netPeer,QByteArray(),1);
    ScpiNotificationSubscriber subscriber2(m_netPeer,QByteArray(),2);
    m_notificationHandler->addSubscriber(subscriber1);
    m_notificationHandler->addSubscriber(subscriber2);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 2);
    m_notificationHandler->removeAllSubscribersFromAPeer(m_netPeer);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 0);
}
