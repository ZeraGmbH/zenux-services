#include "test_hotpluggablecontrollercontainer.h"
#include "testhotplugi2cctrlcommoninfo.h"
#include "testhotplugctrlfactoryi2cctrl.h"
#include "hotpluggablecontrollercontainer.h"
#include "settingscontainer.h"
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <zeramcontrollerbootloaderstopperfactoryfortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_hotpluggablecontrollercontainer);

test_hotpluggablecontrollercontainer::test_hotpluggablecontrollercontainer()
{
}

void test_hotpluggablecontrollercontainer::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
}

void test_hotpluggablecontrollercontainer::initTestCase_data()
{
    QTest::addColumn<QString>("serviceNameForAlternateDevice");
    QTest::newRow("MT310s2") << QString("mt310s2d");
    QTest::newRow("MT581s2") << QString("mt581s2d");
}

void test_hotpluggablecontrollercontainer::init()
{
    QFETCH_GLOBAL(QString, serviceNameForAlternateDevice);
    m_i2cSettings = std::make_unique<TestI2cSettings>(&m_configReader);
    m_senseSettings = std::make_unique<cSenseSettings>(&m_configReader, 8 /*mt310s2*/);

    m_ctrlFactory = std::make_shared<TestHotPlugCtrlFactoryI2cCtrl>(m_i2cSettings.get());
    connect(&m_configReader, &Zera::XMLConfig::cReader::valueChanged,
            m_i2cSettings.get(), &I2cSettings::configXMLInfo);
    connect(&m_configReader, &Zera::XMLConfig::cReader::valueChanged,
            m_senseSettings.get(), &cSenseSettings::configXMLInfo);
    SettingsContainer::TServiceConfig config = SettingsContainer::getServiceConfig(serviceNameForAlternateDevice);
    m_configReader.loadXMLFile(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + config.xmlFileName);
}

void test_hotpluggablecontrollercontainer::initNoController()
{
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    QVector<I2cCtrlCommonInfoPtrShared> controllers = container.getCurrentCommonControllers();
    QCOMPARE(controllers.size(), 0);
}

void test_hotpluggablecontrollercontainer::mt310s2AllVoltageNotPluggable()
{
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    container.startActualizeEmobControllers((1<<0), m_senseSettings.get(), 1000);
    container.startActualizeEmobControllers((1<<1), m_senseSettings.get(), 1000);
    container.startActualizeEmobControllers((1<<2), m_senseSettings.get(), 1000);
    container.startActualizeEmobControllers((1<<3), m_senseSettings.get(), 1000);
    QVector<I2cCtrlCommonInfoPtrShared> controllers = container.getCurrentCommonControllers();
    QCOMPARE(controllers.size(), 0);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1()
{
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    container.startActualizeEmobControllers((1<<4), m_senseSettings.get(), 1000);
    QVector<I2cCtrlCommonInfoPtrShared> controllers = container.getCurrentCommonControllers();
    QCOMPARE(controllers.size(), 1);

}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2()
{
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    container.startActualizeEmobControllers((1<<4) | (1<<5), m_senseSettings.get(), 1000);
    QVector<I2cCtrlCommonInfoPtrShared> controllers = container.getCurrentCommonControllers();
    QCOMPARE(controllers.size(), 2);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1Twice()
{
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    container.startActualizeEmobControllers((1<<4), m_senseSettings.get(), 1000);
    container.startActualizeEmobControllers((1<<4), m_senseSettings.get(), 1000);
    QVector<I2cCtrlCommonInfoPtrShared> controllers = container.getCurrentCommonControllers();
    QCOMPARE(controllers.size(), 1);
    TestHotPlugCtrlFactoryI2cCtrl* factory = static_cast<TestHotPlugCtrlFactoryI2cCtrl*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 1);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2AddI1()
{
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    container.startActualizeEmobControllers((1<<4) | (1<<5), m_senseSettings.get(), 1000);
    container.startActualizeEmobControllers((1<<4), m_senseSettings.get(), 1000);
    QVector<I2cCtrlCommonInfoPtrShared> controllers = container.getCurrentCommonControllers();
    QCOMPARE(controllers.size(), 1);
    TestHotPlugCtrlFactoryI2cCtrl* factory = static_cast<TestHotPlugCtrlFactoryI2cCtrl*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 1);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1CheckI2cSettings()
{
    m_i2cSettings->setI2cAddressesEmob("foo", 1, 2);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    container.startActualizeEmobControllers((1<<4), m_senseSettings.get(), 1000);
    QVector<I2cCtrlCommonInfoPtrShared> controllers = container.getCurrentCommonControllers();
    QCOMPARE(controllers.size(), 1);
    TestHotplugI2cCtrlCommonInfo *testController = static_cast<TestHotplugI2cCtrlCommonInfo*>(controllers[0].get());
    QCOMPARE(testController->getDevnode(), "foo");
    QCOMPARE(testController->getAdrCtrl(), 1);
    QCOMPARE(testController->getAdrMux(), 2);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2I3IAuxCheckMuxSettings()
{
    m_i2cSettings->setI2cAddressesEmob("foo", 1, 2);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    container.startActualizeEmobControllers((1<<4) | (1<<5) | (1<<6) | (1<<7), m_senseSettings.get(), 1000);
    QVector<I2cCtrlCommonInfoPtrShared> controllers = container.getCurrentCommonControllers();
    QCOMPARE(controllers.size(), 4);
    TestHotplugI2cCtrlCommonInfo* ctrlI1 = static_cast<TestHotplugI2cCtrlCommonInfo*>(controllers[0].get());
    QCOMPARE(ctrlI1->getMuxChannel(), 1);
    TestHotplugI2cCtrlCommonInfo* ctrlI2 = static_cast<TestHotplugI2cCtrlCommonInfo*>(controllers[1].get());
    QCOMPARE(ctrlI2->getMuxChannel(), 2);
    TestHotplugI2cCtrlCommonInfo* ctrlI3 = static_cast<TestHotplugI2cCtrlCommonInfo*>(controllers[2].get());
    QCOMPARE(ctrlI3->getMuxChannel(), 3);
    TestHotplugI2cCtrlCommonInfo* ctrlIAux = static_cast<TestHotplugI2cCtrlCommonInfo*>(controllers[3].get());
    QCOMPARE(ctrlIAux->getMuxChannel(), 4);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1CheckSignals()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false);

    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    QSignalSpy spy(&container, &AbstractHotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4), m_senseSettings.get(), 1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 1);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2CheckSignalsImmediate()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true << true);

    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    QSignalSpy spy(&container, &AbstractHotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4) | (1<<5), m_senseSettings.get(), 1000);
    QCOMPARE(spy.count(), 2);
    TestHotPlugCtrlFactoryI2cCtrl* factory = static_cast<TestHotPlugCtrlFactoryI2cCtrl*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 2);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2CheckSignalsDelayed()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false << false);

    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    QSignalSpy spy(&container, &AbstractHotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4) | (1<<5), m_senseSettings.get(), 1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(1000);
    TestHotPlugCtrlFactoryI2cCtrl* factory = static_cast<TestHotPlugCtrlFactoryI2cCtrl*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 2);
    QCOMPARE(spy.count(), 2);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1AndRemoveBeforeFinish()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false);

    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    QSignalSpy spy(&container, &AbstractHotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4), m_senseSettings.get(), 1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(500);
    container.startActualizeEmobControllers(0, m_senseSettings.get(), 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 0);
    TestHotPlugCtrlFactoryI2cCtrl* factory = static_cast<TestHotPlugCtrlFactoryI2cCtrl*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 0);
    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1AndAddI2BeforeFinish()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false << false);

    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    QSignalSpy spy(&container, &AbstractHotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers((1<<4), m_senseSettings.get(), 1000);
    QCOMPARE(spy.count(), 0);
    
    TestHotPlugCtrlFactoryI2cCtrl* factory = static_cast<TestHotPlugCtrlFactoryI2cCtrl*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 0);

    TimeMachineForTest::getInstance()->processTimers(500); // 500
    container.startActualizeEmobControllers((1<<4) | (1<<5), m_senseSettings.get(), 1000);
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
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    QSignalSpy spy(&container, &AbstractHotPluggableControllerContainer::sigControllersChanged);
    
    TestHotPlugCtrlFactoryI2cCtrl* factory = static_cast<TestHotPlugCtrlFactoryI2cCtrl*>(m_ctrlFactory.get());

    // add clamp only
    factory->prepareNextTestControllers(QVector<bool>() << false);
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true);
    container.startActualizeEmobControllers((1<<4), m_senseSettings.get(), 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(factory->getCtrlInstanceCount(), 0);

    // add emob to clamp
    factory->prepareNextTestControllers(QVector<bool>() << true); // we expect clamp known => no version query
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true); // same
    container.startActualizeEmobControllers((1<<4) | (1<<5), m_senseSettings.get(), 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 1);
    spy.clear();
    QCOMPARE(factory->getCtrlInstanceCount(), 1);

    // remove clamp
    container.startActualizeEmobControllers((1<<5), m_senseSettings.get(), 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(factory->getCtrlInstanceCount(), 1);

    // remove emob
    container.startActualizeEmobControllers(0, m_senseSettings.get(), 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 1);
    spy.clear();
    QCOMPARE(factory->getCtrlInstanceCount(), 0);

    // add clamp & emob only
    factory->prepareNextTestControllers(QVector<bool>() << false << true);
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true << true);
    container.startActualizeEmobControllers((1<<4) | (1<<5), m_senseSettings.get(), 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(factory->getCtrlInstanceCount(), 1);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);
}
