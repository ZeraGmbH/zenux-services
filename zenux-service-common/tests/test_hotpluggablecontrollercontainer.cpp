#include "test_hotpluggablecontrollercontainer.h"
#include "atmelctrlfactoryfortest.h"
#include "atmelemobctrlfortest.h"
#include "hotpluggablecontrollercontainer.h"
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <zeramcontrollerbootloaderstopperfactoryfortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_hotpluggablecontrollercontainer);

test_hotpluggablecontrollercontainer::test_hotpluggablecontrollercontainer() :
    m_senseSettings(&m_configReader, 8 /*mt310s2*/)
{
    connect(&m_configReader, &Zera::XMLConfig::cReader::valueChanged,
            &m_senseSettings, &cSenseSettings::configXMLInfo);
    m_configReader.loadSchema(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xsd");
    m_configReader.loadXMLFile(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xml");
}

void test_hotpluggablecontrollercontainer::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
}

void test_hotpluggablecontrollercontainer::initNoController()
{
    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    QVector<AtmelCommonVersionsPtrS> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 0);
}

void test_hotpluggablecontrollercontainer::mt310s2AllVoltageNotPluggable()
{
    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    container.startActualizeEmobControllers((1<<0), &m_senseSettings, 1000);
    container.startActualizeEmobControllers((1<<1), &m_senseSettings, 1000);
    container.startActualizeEmobControllers((1<<2), &m_senseSettings, 1000);
    container.startActualizeEmobControllers((1<<3), &m_senseSettings, 1000);
    QVector<AtmelCommonVersionsPtrS> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 0);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1()
{
    AtmelCtrlFactoryForTest::prepareNextTestControllers(QVector<bool>() << true);
    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    QVector<AtmelCommonVersionsPtrS> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);

}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2()
{
    AtmelCtrlFactoryForTest::prepareNextTestControllers(QVector<bool>() << true << true);
    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    container.startActualizeEmobControllers((1<<4) | (1<<5), &m_senseSettings, 1000);
    QVector<AtmelCommonVersionsPtrS> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 2);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1Twice()
{
    AtmelCtrlFactoryForTest::prepareNextTestControllers(QVector<bool>() << true);
    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    QVector<AtmelCommonVersionsPtrS> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 1);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2AddI1()
{
    AtmelCtrlFactoryForTest::prepareNextTestControllers(QVector<bool>() << true);
    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    container.startActualizeEmobControllers((1<<4) | (1<<5), &m_senseSettings, 1000);
    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    QVector<AtmelCommonVersionsPtrS> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 1);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1CheckI2cSettings()
{
    AtmelCtrlFactoryForTest::prepareNextTestControllers(QVector<bool>() << true);
    HotPluggableControllerContainer container("foo", 1, 2, 3);
    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    QVector<AtmelCommonVersionsPtrS> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    AtmelEmobCtrlForTest* ctrl = static_cast<AtmelEmobCtrlForTest*>(controllers[0].get());
    QCOMPARE(ctrl->getDevnode(), "foo");
    QCOMPARE(ctrl->getAdrCtrl(), 1);
    QCOMPARE(ctrl->getAdrMux(), 2);
    QCOMPARE(ctrl->getDebuglevel(), 3);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2I3IAuxCheckMuxSettings()
{
    AtmelCtrlFactoryForTest::prepareNextTestControllers(QVector<bool>() << true << true << true << true);
    HotPluggableControllerContainer container("foo", 1, 2, 3);
    container.startActualizeEmobControllers((1<<4) | (1<<5) | (1<<6) | (1<<7), &m_senseSettings, 1000);
    QVector<AtmelCommonVersionsPtrS> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 4);
    AtmelEmobCtrlForTest* ctrlI1 = static_cast<AtmelEmobCtrlForTest*>(controllers[0].get());
    QCOMPARE(ctrlI1->getMuxChannel(), 1);
    AtmelEmobCtrlForTest* ctrlI2 = static_cast<AtmelEmobCtrlForTest*>(controllers[1].get());
    QCOMPARE(ctrlI2->getMuxChannel(), 2);
    AtmelEmobCtrlForTest* ctrlI3 = static_cast<AtmelEmobCtrlForTest*>(controllers[2].get());
    QCOMPARE(ctrlI3->getMuxChannel(), 3);
    AtmelEmobCtrlForTest* ctrlIAux = static_cast<AtmelEmobCtrlForTest*>(controllers[3].get());
    QCOMPARE(ctrlIAux->getMuxChannel(), 4);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1CheckSignals()
{
    AtmelCtrlFactoryForTest::prepareNextTestControllers(QVector<bool>() << true);
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false);

    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 1);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2CheckSignalsImmediate()
{
    AtmelCtrlFactoryForTest::prepareNextTestControllers(QVector<bool>() << true << true);
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true << true);

    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4) | (1<<5), &m_senseSettings, 1000);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 2);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2CheckSignalsDelayed()
{
    AtmelCtrlFactoryForTest::prepareNextTestControllers(QVector<bool>() << true << true);
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false << false);

    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4) | (1<<5), &m_senseSettings, 1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 2);
    QCOMPARE(spy.count(), 2);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1AndRemoveBeforeFinish()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false);

    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(500);
    container.startActualizeEmobControllers(0, &m_senseSettings, 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 0);
    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1AndAddI2BeforeFinish()
{
    AtmelCtrlFactoryForTest::prepareNextTestControllers(QVector<bool>() << true << true);
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false << false);

    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 0);

    TimeMachineForTest::getInstance()->processTimers(500); // 500
    container.startActualizeEmobControllers((1<<4) | (1<<5), &m_senseSettings, 1000);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 0);

    TimeMachineForTest::getInstance()->processTimers(500); // 1000
    QCOMPARE(spy.count(), 1);
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 1);

    TimeMachineForTest::getInstance()->processTimers(500); // 1500
    QCOMPARE(spy.count(), 2);
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 2);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_hotpluggablecontrollercontainer::mt310s2AddClampNoController()
{
    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    // add clamp only
    AtmelCtrlFactoryForTest::prepareNextTestControllers(QVector<bool>() << false);
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true);
    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 0);

    // add emob to clamp
    AtmelCtrlFactoryForTest::prepareNextTestControllers(QVector<bool>() << true); // we expect clamp known => no version query
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true); // same
    container.startActualizeEmobControllers((1<<4) | (1<<5), &m_senseSettings, 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 1);
    spy.clear();
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 1);

    // remove clamp
    container.startActualizeEmobControllers((1<<5), &m_senseSettings, 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 1);

    // remove emob
    container.startActualizeEmobControllers(0, &m_senseSettings, 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 1);
    spy.clear();
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 0);

    // add clamp & emob only
    AtmelCtrlFactoryForTest::prepareNextTestControllers(QVector<bool>() << false << true);
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true << true);
    container.startActualizeEmobControllers((1<<4) | (1<<5), &m_senseSettings, 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 1);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}
