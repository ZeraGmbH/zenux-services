#include "test_notificationsubscriber.h"
#include <QTest>
#include <memory>

QTEST_MAIN(test_notificationsubscriber);

void test_notificationsubscriber::init()
{
    m_subscriber = std::make_unique<ScpiNotificationSubscriber>(nullptr,QByteArray(),1);
}

void test_notificationsubscriber::addAndRemoveSubscriber()
{
    m_notificationHandler.addSubscriber(*m_subscriber);
    QCOMPARE(m_notificationHandler.getTotalSubscribers(), 1);
    QVERIFY(m_notificationHandler.removeSubscriber(*m_subscriber));
    QCOMPARE(m_notificationHandler.getTotalSubscribers(), 0);
}

void test_notificationsubscriber::removeNonAddedSubscriber()
{
    QVERIFY(!m_notificationHandler.removeSubscriber(*m_subscriber));
}

void test_notificationsubscriber::addSubscriberTwice()
{
    m_notificationHandler.addSubscriber(*m_subscriber);
    QVERIFY(!m_notificationHandler.addSubscriber(*m_subscriber));
    QCOMPARE(m_notificationHandler.getTotalSubscribers(), 1);
}
