#include "test_booloaderstopper.h"
#include "testfactoryi2cctrl.h"
#include "i2cctlbootloaderstopper.h"
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_booloaderstopper)

void test_booloaderstopper::init()
{
    TimerFactoryQtForTest::enableTest();
    TimeMachineForTest::reset();
}

void test_booloaderstopper::applicationRunning()
{
    TestFactoryI2cCtrlPtr ctrlFactory = std::make_shared<TestFactoryI2cCtrl>(true);
    ctrlFactory->simulateApplicationRunnung();

    I2cCtlBootloaderStopper stopper(ctrlFactory, 42);
    QSignalSpy spy(&stopper, &I2cCtlBootloaderStopper::sigAssumeBootloaderStopped);

    // Application running: notify immediately
    stopper.stopBootloader(1000);
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 42);
}

void test_booloaderstopper::bootloaderRunning()
{
    TestFactoryI2cCtrlPtr ctrlFactory = std::make_shared<TestFactoryI2cCtrl>(true);
    ctrlFactory->simulateBootloaderRunning();
    I2cCtlBootloaderStopper stopper(ctrlFactory, 67);
    QSignalSpy spy(&stopper, &I2cCtlBootloaderStopper::sigAssumeBootloaderStopped);

    // Bootloader running: notify after delay
    stopper.stopBootloader(1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(500);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(500);
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 67);
}
