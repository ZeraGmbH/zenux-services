#include "test_hotpluggablecontrollercontainer.h"
#include "emobctrlfactoryfortest.h"
#include "hotplugctrltest.h"
#include "hotplugctrltestfactory.h"
#include "hotpluggablecontrollercontainer.h"
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <zeramcontrollerbootloaderstopperfactoryfortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_hotpluggablecontrollercontainer);

test_hotpluggablecontrollercontainer::test_hotpluggablecontrollercontainer() :
    m_i2cSettings(&m_configReader),
    m_senseSettings(&m_configReader, 8 /*mt310s2*/)
{
    connect(&m_configReader, &Zera::XMLConfig::cReader::valueChanged,
            &m_i2cSettings, &I2cSettings::configXMLInfo);
    connect(&m_configReader, &Zera::XMLConfig::cReader::valueChanged,
            &m_senseSettings, &cSenseSettings::configXMLInfo);
    m_configReader.loadSchema(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xsd");
    m_configReader.loadXMLFile(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xml");
}

void test_hotpluggablecontrollercontainer::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
}

void test_hotpluggablecontrollercontainer::init()
{
    m_ctrlFactory = std::make_shared<HotPlugCtrlTestFactory>(&m_i2cSettings);
}

void test_hotpluggablecontrollercontainer::initNoController()
{
    m_i2cSettings.setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(&m_i2cSettings, m_ctrlFactory);
    QVector<I2cCtrlCommonVersionsPtrShared> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 0);
}

void test_hotpluggablecontrollercontainer::mt310s2AllVoltageNotPluggable()
{
    m_i2cSettings.setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(&m_i2cSettings, m_ctrlFactory);
    container.startActualizeEmobControllers((1<<0), &m_senseSettings, 1000);
    container.startActualizeEmobControllers((1<<1), &m_senseSettings, 1000);
    container.startActualizeEmobControllers((1<<2), &m_senseSettings, 1000);
    container.startActualizeEmobControllers((1<<3), &m_senseSettings, 1000);
    QVector<I2cCtrlCommonVersionsPtrShared> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 0);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1()
{
    m_i2cSettings.setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(&m_i2cSettings, m_ctrlFactory);
    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    QVector<I2cCtrlCommonVersionsPtrShared> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);

}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2()
{
    m_i2cSettings.setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(&m_i2cSettings, m_ctrlFactory);
    container.startActualizeEmobControllers((1<<4) | (1<<5), &m_senseSettings, 1000);
    QVector<I2cCtrlCommonVersionsPtrShared> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 2);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1Twice()
{
    m_i2cSettings.setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(&m_i2cSettings, m_ctrlFactory);
    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    QVector<I2cCtrlCommonVersionsPtrShared> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    HotPlugCtrlTestFactory* factory = static_cast<HotPlugCtrlTestFactory*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 1);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2AddI1()
{
    m_i2cSettings.setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(&m_i2cSettings, m_ctrlFactory);
    container.startActualizeEmobControllers((1<<4) | (1<<5), &m_senseSettings, 1000);
    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    QVector<I2cCtrlCommonVersionsPtrShared> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    HotPlugCtrlTestFactory* factory = static_cast<HotPlugCtrlTestFactory*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 1);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1CheckI2cSettings()
{
    m_i2cSettings.setI2cAddressesEmob("foo", 1, 2);
    HotPluggableControllerContainer container(&m_i2cSettings, m_ctrlFactory);
    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    QVector<I2cCtrlCommonVersionsPtrShared> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    HotPlugCtrlTest *testController = static_cast<HotPlugCtrlTest*>(controllers[0].get());
    QCOMPARE(testController->getDevnode(), "foo");
    QCOMPARE(testController->getAdrCtrl(), 1);
    QCOMPARE(testController->getAdrMux(), 2);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2I3IAuxCheckMuxSettings()
{
    m_i2cSettings.setI2cAddressesEmob("foo", 1, 2);
    HotPluggableControllerContainer container(&m_i2cSettings, m_ctrlFactory);
    container.startActualizeEmobControllers((1<<4) | (1<<5) | (1<<6) | (1<<7), &m_senseSettings, 1000);
    QVector<I2cCtrlCommonVersionsPtrShared> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 4);
    HotPlugCtrlTest* ctrlI1 = static_cast<HotPlugCtrlTest*>(controllers[0].get());
    QCOMPARE(ctrlI1->getMuxChannel(), 1);
    HotPlugCtrlTest* ctrlI2 = static_cast<HotPlugCtrlTest*>(controllers[1].get());
    QCOMPARE(ctrlI2->getMuxChannel(), 2);
    HotPlugCtrlTest* ctrlI3 = static_cast<HotPlugCtrlTest*>(controllers[2].get());
    QCOMPARE(ctrlI3->getMuxChannel(), 3);
    HotPlugCtrlTest* ctrlIAux = static_cast<HotPlugCtrlTest*>(controllers[3].get());
    QCOMPARE(ctrlIAux->getMuxChannel(), 4);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1CheckSignals()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false);

    m_i2cSettings.setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(&m_i2cSettings, m_ctrlFactory);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 1);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2CheckSignalsImmediate()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true << true);

    m_i2cSettings.setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(&m_i2cSettings, m_ctrlFactory);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4) | (1<<5), &m_senseSettings, 1000);
    QCOMPARE(spy.count(), 2);
    HotPlugCtrlTestFactory* factory = static_cast<HotPlugCtrlTestFactory*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 2);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2CheckSignalsDelayed()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false << false);

    m_i2cSettings.setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(&m_i2cSettings, m_ctrlFactory);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4) | (1<<5), &m_senseSettings, 1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(1000);
    HotPlugCtrlTestFactory* factory = static_cast<HotPlugCtrlTestFactory*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 2);
    QCOMPARE(spy.count(), 2);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1AndRemoveBeforeFinish()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false);

    m_i2cSettings.setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(&m_i2cSettings, m_ctrlFactory);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(500);
    container.startActualizeEmobControllers(0, &m_senseSettings, 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 0);
    HotPlugCtrlTestFactory* factory = static_cast<HotPlugCtrlTestFactory*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 0);
    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1AndAddI2BeforeFinish()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false << false);

    m_i2cSettings.setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(&m_i2cSettings, m_ctrlFactory);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    QCOMPARE(spy.count(), 0);

    HotPlugCtrlTestFactory* factory = static_cast<HotPlugCtrlTestFactory*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 0);

    TimeMachineForTest::getInstance()->processTimers(500); // 500
    container.startActualizeEmobControllers((1<<4) | (1<<5), &m_senseSettings, 1000);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(factory->getCtrlInstanceCount(), 0);

    TimeMachineForTest::getInstance()->processTimers(500); // 1000
    QCOMPARE(spy.count(), 1);
    QCOMPARE(factory->getCtrlInstanceCount(), 1);

    TimeMachineForTest::getInstance()->processTimers(500); // 1500
    QCOMPARE(spy.count(), 2);
    QCOMPARE(factory->getCtrlInstanceCount(), 2);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_hotpluggablecontrollercontainer::mt310s2AddClampNoController()
{
    m_i2cSettings.setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(&m_i2cSettings, m_ctrlFactory);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    HotPlugCtrlTestFactory* factory = static_cast<HotPlugCtrlTestFactory*>(m_ctrlFactory.get());

    // add clamp only
    factory->prepareNextTestControllers(QVector<bool>() << false);
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true);
    container.startActualizeEmobControllers((1<<4), &m_senseSettings, 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(factory->getCtrlInstanceCount(), 0);

    // add emob to clamp
    factory->prepareNextTestControllers(QVector<bool>() << true); // we expect clamp known => no version query
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true); // same
    container.startActualizeEmobControllers((1<<4) | (1<<5), &m_senseSettings, 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 1);
    spy.clear();
    QCOMPARE(factory->getCtrlInstanceCount(), 1);

    // remove clamp
    container.startActualizeEmobControllers((1<<5), &m_senseSettings, 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(factory->getCtrlInstanceCount(), 1);

    // remove emob
    container.startActualizeEmobControllers(0, &m_senseSettings, 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 1);
    spy.clear();
    QCOMPARE(factory->getCtrlInstanceCount(), 0);

    // add clamp & emob only
    factory->prepareNextTestControllers(QVector<bool>() << false << true);
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true << true);
    container.startActualizeEmobControllers((1<<4) | (1<<5), &m_senseSettings, 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(factory->getCtrlInstanceCount(), 1);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}
