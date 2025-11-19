#include "test_hotpluggablecontrollercontainer.h"
#include "controllerpersitentdata.h"
#include "testfactoryi2cctrl.h"
#include "testhotplugi2cctrlcommoninfo.h"
#include "testhotplugctrlfactoryi2cctrl.h"
#include "hotpluggablecontrollercontainer.h"
#include "settingscontainer.h"
#include <mocktcpnetworkfactory.h>
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <zeramcontrollerbootloaderstopperfactoryfortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_hotpluggablecontrollercontainer);

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

void test_hotpluggablecontrollercontainer::cleanup()
{
    m_mt310s2d = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
    ControllerPersitentData::cleanupPersitentData();
    ZeraMControllerBootloaderStopperFactoryForTest::cleanup();
}

void test_hotpluggablecontrollercontainer::initNoController()
{
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 0);
}

void test_hotpluggablecontrollercontainer::mt310s2AllVoltageNotPluggable()
{
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    container.startActualizeEmobControllers(getChannelPlugMask("UL1"), m_senseSettings.get(), 1000);
    container.startActualizeEmobControllers(getChannelPlugMask("UL2"), m_senseSettings.get(), 1000);
    container.startActualizeEmobControllers(getChannelPlugMask("UL3"), m_senseSettings.get(), 1000);
    container.startActualizeEmobControllers(getChannelPlugMask("UAUX"), m_senseSettings.get(), 1000);
    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 0);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1()
{
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    container.startActualizeEmobControllers(getChannelPlugMask("IL1"), m_senseSettings.get(), 1000);
    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    QVERIFY(controllers.contains(getChannelMName("IL1")));
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2()
{
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    container.startActualizeEmobControllers(getChannelPlugMask("IL1") | getChannelPlugMask("IL2"), m_senseSettings.get(), 1000);
    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 2);
    QVERIFY(controllers.contains(getChannelMName("IL1")));
    QVERIFY(controllers.contains(getChannelMName("IL2")));
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1Twice()
{
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    container.startActualizeEmobControllers(getChannelPlugMask("IL1"), m_senseSettings.get(), 1000);
    container.startActualizeEmobControllers(getChannelPlugMask("IL1"), m_senseSettings.get(), 1000);
    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    TestHotPlugCtrlFactoryI2cCtrl* factory = static_cast<TestHotPlugCtrlFactoryI2cCtrl*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 1);
    QVERIFY(controllers.contains(getChannelMName("IL1")));
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2RemoveI2()
{
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    container.startActualizeEmobControllers(getChannelPlugMask("IL1") | getChannelPlugMask("IL2"), m_senseSettings.get(), 1000);
    container.startActualizeEmobControllers(getChannelPlugMask("IL1"), m_senseSettings.get(), 1000);
    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    TestHotPlugCtrlFactoryI2cCtrl* factory = static_cast<TestHotPlugCtrlFactoryI2cCtrl*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 1);
    QVERIFY(controllers.contains(getChannelMName("IL1")));
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1CheckI2cSettings()
{
    m_i2cSettings->setI2cAddressesEmob("foo", 1, 2);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    container.startActualizeEmobControllers(getChannelPlugMask("IL1"), m_senseSettings.get(), 1000);
    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    QVERIFY(controllers.contains(getChannelMName("IL1")));
    HotControllers controller = controllers[getChannelMName("IL1")];
    TestHotplugI2cCtrlCommonInfo *testController = static_cast<TestHotplugI2cCtrlCommonInfo*>(controller.m_commonController.get());
    QCOMPARE(testController->getDevnode(), "foo");
    QCOMPARE(testController->getAdrCtrl(), 1);
    QCOMPARE(testController->getAdrMux(), 2);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2I3IAuxCheckMuxSettings()
{
    m_i2cSettings->setI2cAddressesEmob("foo", 1, 2);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    container.startActualizeEmobControllers(getChannelPlugMask("IL1") |
                                            getChannelPlugMask("IL2") |
                                            getChannelPlugMask("IL3") |
                                            getChannelPlugMask("IAUX"), m_senseSettings.get(), 1000);
    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 4);
    TestHotplugI2cCtrlCommonInfo* ctrlI1 = static_cast<TestHotplugI2cCtrlCommonInfo*>(controllers[getChannelMName("IL1")].m_commonController.get());
    QCOMPARE(ctrlI1->getMuxChannel(), 1);
    TestHotplugI2cCtrlCommonInfo* ctrlI2 = static_cast<TestHotplugI2cCtrlCommonInfo*>(controllers[getChannelMName("IL2")].m_commonController.get());
    QCOMPARE(ctrlI2->getMuxChannel(), 2);
    TestHotplugI2cCtrlCommonInfo* ctrlI3 = static_cast<TestHotplugI2cCtrlCommonInfo*>(controllers[getChannelMName("IL3")].m_commonController.get());
    QCOMPARE(ctrlI3->getMuxChannel(), 3);
    TestHotplugI2cCtrlCommonInfo* ctrlIAux = static_cast<TestHotplugI2cCtrlCommonInfo*>(controllers[getChannelMName("IAUX")].m_commonController.get());
    QCOMPARE(ctrlIAux->getMuxChannel(), 4);
    QVERIFY(controllers.contains(getChannelMName("IL1")));
    QVERIFY(controllers.contains(getChannelMName("IL2")));
    QVERIFY(controllers.contains(getChannelMName("IL3")));
    QVERIFY(controllers.contains(getChannelMName("IAUX")));
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1CheckSignals()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false);

    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers(getChannelPlugMask("IL1"), m_senseSettings.get(), 1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 1);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);

    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    QVERIFY(controllers.contains(getChannelMName("IL1")));
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2CheckSignalsImmediate()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << true << true);

    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers(getChannelPlugMask("IL1") | getChannelPlugMask("IL2"), m_senseSettings.get(), 1000);
    QCOMPARE(spy.count(), 2);
    TestHotPlugCtrlFactoryI2cCtrl* factory = static_cast<TestHotPlugCtrlFactoryI2cCtrl*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 2);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);

    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 2);
    QVERIFY(controllers.contains(getChannelMName("IL1")));
    QVERIFY(controllers.contains(getChannelMName("IL2")));
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2CheckSignalsDelayed()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false << false);

    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers(getChannelPlugMask("IL1") | getChannelPlugMask("IL2"), m_senseSettings.get(), 1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(1000);
    TestHotPlugCtrlFactoryI2cCtrl* factory = static_cast<TestHotPlugCtrlFactoryI2cCtrl*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 2);
    QCOMPARE(spy.count(), 2);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);

    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 2);
    QVERIFY(controllers.contains(getChannelMName("IL1")));
    QVERIFY(controllers.contains(getChannelMName("IL2")));
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1AndRemoveBeforeFinish()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false);

    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers(getChannelPlugMask("IL1"), m_senseSettings.get(), 1000);
    QCOMPARE(spy.count(), 0);
    TimeMachineForTest::getInstance()->processTimers(500);
    container.startActualizeEmobControllers(0, m_senseSettings.get(), 1000);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 0);
    TestHotPlugCtrlFactoryI2cCtrl* factory = static_cast<TestHotPlugCtrlFactoryI2cCtrl*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 0);
    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);

    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 0);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1AndAddI2BeforeFinish()
{
    ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool>() << false << false);

    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    QSignalSpy spy(&container, &HotPluggableControllerContainer::sigControllersChanged);

    container.startActualizeEmobControllers(getChannelPlugMask("IL1"), m_senseSettings.get(), 1000);
    QCOMPARE(spy.count(), 0);
    
    TestHotPlugCtrlFactoryI2cCtrl* factory = static_cast<TestHotPlugCtrlFactoryI2cCtrl*>(m_ctrlFactory.get());
    QCOMPARE(factory->getCtrlInstanceCount(), 0);

    TimeMachineForTest::getInstance()->processTimers(500); // 500
    container.startActualizeEmobControllers(getChannelPlugMask("IL1") | getChannelPlugMask("IL2"), m_senseSettings.get(), 1000);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(factory->getCtrlInstanceCount(), 0);

    TimeMachineForTest::getInstance()->processTimers(500); // 1000
    QCOMPARE(spy.count(), 1);
    QCOMPARE(factory->getCtrlInstanceCount(), 1);

    TimeMachineForTest::getInstance()->processTimers(500); // 1500
    QCOMPARE(spy.count(), 2);
    QCOMPARE(factory->getCtrlInstanceCount(), 2);

    QCOMPARE(ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty(), true);

    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 2);
    QVERIFY(controllers.contains(getChannelMName("IL1")));
    QVERIFY(controllers.contains(getChannelMName("IL2")));
}

void test_hotpluggablecontrollercontainer::mt310s2AddClampNoController()
{
    // use m_mt310s2d->fireHotplugInterruptControllerName in here
    createServers();
    HotPluggableControllerContainerPtr container = m_mt310s2d->getHotPluggableControllerContainer();
    QSignalSpy spy(container.get(), &HotPluggableControllerContainer::sigControllersChanged);

    // add clamp only IL1
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"", cClamp::CL1000A});
    m_mt310s2d->fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 0);
    HotControllerMap controllers = container->getCurrentControllers();
    QCOMPARE(controllers.size(), 0);
    controllers.clear();

    // add emob to clamp IL2
    infoMap.insert("IL2", {"Emob", cClamp::CL1000A});
    m_mt310s2d->fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
    spy.clear();
    controllers = container->getCurrentControllers();
    QCOMPARE(controllers.size(), 1); // IL1 clamp only / IL2 with controller added
    QVERIFY(controllers.contains(getChannelMName("IL2")));
    controllers.clear();

    // remove clamp IL1
    infoMap.remove("IL1");
    m_mt310s2d->fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 0);
    controllers = container->getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    QVERIFY(controllers.contains(getChannelMName("IL2")));
    controllers.clear();

    // remove all
    infoMap.clear();
    m_mt310s2d->fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
    spy.clear();
    controllers = container->getCurrentControllers();
    QCOMPARE(controllers.size(), 0);
    controllers.clear();

    // add clamp IL1 & emob only IL2
    infoMap.insert("IL1", {"", cClamp::CL1000A});
    infoMap.insert("IL2", {"Emob", cClamp::undefined});
    m_mt310s2d->fireHotplugInterruptControllerName(infoMap);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spy.count(), 1);
    controllers = container->getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    QVERIFY(controllers.contains(getChannelMName("IL2")));
    controllers.clear();
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1InstrumentDetectsMt650e()
{
    const QString channelAlias = "IL1";
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    ControllerPersitentData::MuxChannelDeviceNameMap deviceMap;
    deviceMap.insert(getChannelMuxChannel(channelAlias), {"MT650e", cClamp::ClampTypes::undefined});
    ControllerPersitentData::setHotplugDevices(deviceMap);

    container.startActualizeEmobControllers(getChannelPlugMask(channelAlias), m_senseSettings.get(), 1000);
    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    QVERIFY(controllers.contains(getChannelMName(channelAlias)));
    QCOMPARE(controllers[getChannelMName(channelAlias)].m_controllerType, MT650e);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1InstrumentDefaultsEmob()
{
    const QString channelAlias = "IL1";
    m_i2cSettings->setI2cAddressesEmob(QString(), 0, 0);
    HotPluggableControllerContainer container(m_i2cSettings.get(), m_ctrlFactory);
    ControllerPersitentData::MuxChannelDeviceNameMap deviceMap;
    deviceMap.insert(getChannelMuxChannel(channelAlias), {"foo", cClamp::ClampTypes::undefined});
    ControllerPersitentData::setHotplugDevices(deviceMap);

    container.startActualizeEmobControllers(getChannelPlugMask(channelAlias), m_senseSettings.get(), 1000);
    HotControllerMap controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    QVERIFY(controllers.contains(getChannelMName(channelAlias)));
    QCOMPARE(controllers[getChannelMName(channelAlias)].m_controllerType, EMOB);
}

quint16 test_hotpluggablecontrollercontainer::getChannelPlugMask(const QString &channelAlias)
{
    SenseSystem::cChannelSettings* channelSettings = m_senseSettings->findChannelSettingByAlias1(channelAlias);
    return (1<<channelSettings->m_nPluggedBit);
}

QString test_hotpluggablecontrollercontainer::getChannelMName(const QString &channelAlias)
{
    SenseSystem::cChannelSettings* channelSettings = m_senseSettings->findChannelSettingByAlias1(channelAlias);
    return channelSettings->m_nameMx;
}

quint8 test_hotpluggablecontrollercontainer::getChannelMuxChannel(const QString &channelAlias)
{
    SenseSystem::cChannelSettings* channelSettings = m_senseSettings->findChannelSettingByAlias1(channelAlias);
    return channelSettings->m_nMuxChannelNo;
}

void test_hotpluggablecontrollercontainer::createServers()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_mt310s2d = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), tcpNetworkFactory, "mt310s2d");
    TimeMachineObject::feedEventLoop();
}
