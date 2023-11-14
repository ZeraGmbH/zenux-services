#include "test_interprocessnotifier.h"
#include "interprocessnotifier.h"
#include "QSignalSpy"
#include "QTest"

QTEST_MAIN(test_interprocessnotifier);

void test_interprocessnotifier::openOnce()
{
    InterProcessNotifier notifier;
    QCOMPARE(notifier.open(), true);
}

void test_interprocessnotifier::openTwice()
{
    InterProcessNotifier notifier;
    QCOMPARE(notifier.open(), true);
    QCOMPARE(notifier.open(), false);
}

void test_interprocessnotifier::openCloseOpen()
{
    InterProcessNotifier notifier;
    QCOMPARE(notifier.open(), true);
    notifier.close();
    QCOMPARE(notifier.open(), true);
}

void test_interprocessnotifier::openCloseTwiceOpen()
{
    InterProcessNotifier notifier;
    QCOMPARE(notifier.open(), true);
    notifier.close();
    notifier.close();
    QCOMPARE(notifier.open(), true);
}

// Will the following turn fragile??
// Hope we just test business logic 'working' for years...

void test_interprocessnotifier::sendOneSignal()
{
    InterProcessNotifier notifier;
    QCOMPARE(notifier.open(), true);
    QSignalSpy spy(&notifier, &InterProcessNotifier::sigSignal);

    notifier.sendSignal();
    QTest::qWait(1);
    QCOMPARE(spy.count(), 1);
}

void test_interprocessnotifier::sendTwoSignals()
{
    InterProcessNotifier notifier;
    QCOMPARE(notifier.open(), true);
    QSignalSpy spy(&notifier, &InterProcessNotifier::sigSignal);

    notifier.sendSignal();
    QTest::qWait(1);
    QCOMPARE(spy.count(), 1);

    notifier.sendSignal();
    QTest::qWait(1);
    QCOMPARE(spy.count(), 2);
}
