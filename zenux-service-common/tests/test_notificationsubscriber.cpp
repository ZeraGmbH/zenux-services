#include "test_notificationsubscriber.h"
#include <QTest>
#include <memory>

QTEST_MAIN(test_notificationsubscriber);

void test_notificationsubscriber::init()
{
    XiQNetPeer *netPeer = new XiQNetPeer();
    m_subscriberA = std::make_unique<ScpiNotificationSubscriber>(nullptr,QByteArray(),1);
    m_subscriberB = std::make_unique<ScpiNotificationSubscriber>(netPeer,QByteArray(),2);
    m_notificationHandler = new ScpiNotificationSubscriberHandler();
}

void test_notificationsubscriber::cleanup()
{
    delete m_notificationHandler;
}

void test_notificationsubscriber::addAndRemoveSubscriber()
{
    m_notificationHandler->addSubscriber(*m_subscriberA);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 1);
    QVERIFY(m_notificationHandler->removeSubscriber(*m_subscriberA));
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 0);
}

void test_notificationsubscriber::removeNonAddedSubscriber()
{
    QVERIFY(!m_notificationHandler->removeSubscriber(*m_subscriberA));
}

void test_notificationsubscriber::addSubscriberTwice()
{
    m_notificationHandler->addSubscriber(*m_subscriberA);
    QVERIFY(!m_notificationHandler->addSubscriber(*m_subscriberA));
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 1);
}

void test_notificationsubscriber::addTwoSubscribers()
{
    m_notificationHandler->addSubscriber(*m_subscriberA);
    m_notificationHandler->addSubscriber(*m_subscriberB);
    QCOMPARE(m_notificationHandler->getTotalSubscribers(), 2);
}
