#include "test_onofffilewatcher.h"
#include "onofffilewatcher.h"
#include "testonofffilegenerator.h"
#include <timemachineobject.h>
#include "QFile"
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_onofffilewatcher)

void test_onofffilewatcher::init()
{
    TestOnOffFileGenerator::deleteOnFile();
}

void test_onofffilewatcher::initialOff()
{
    OnOffFileWatcher watcher;
    QCOMPARE(watcher.isOn(), false);
}

void test_onofffilewatcher::initialOn()
{
    OnOffFileWatcher watcher;
    TestOnOffFileGenerator::createOnFile();
    QCOMPARE(watcher.isOn(), true);
}

void test_onofffilewatcher::dynamicOffOnOff()
{
    OnOffFileWatcher watcher;
    QSignalSpy spy(&watcher, &OnOffFileWatcher::sigOnOffChangeDetected);

    QCOMPARE(watcher.isOn(), false);

    TestOnOffFileGenerator::createOnFile();
    QCOMPARE(watcher.isOn(), true);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);

    TestOnOffFileGenerator::deleteOnFile();
    QCOMPARE(watcher.isOn(), false);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 2);

    TestOnOffFileGenerator::createOnFile();
    QCOMPARE(watcher.isOn(), true);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 3);
}
