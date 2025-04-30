#include "test_authorizationnotifier.h"
#include "testadjustmentstatusinterfacenull.h"
#include "servicestatusinterface.h"
#include "testfactoryi2cctrl.h"
#include "testi2cctrleeprompermission.h"
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <mocktcpnetworkfactory.h>
#include <QTest>
#include <QSignalSpy>

QTEST_MAIN(test_authorizationnotifier)

static const char *statusAuthorizationCommand = "STATUS:AUTHORIZATION?";
static const char *registerNotifierCommand = "SERVER:REGISTER";
static const char *unregisterNotifierCommand = "SERVER:UNREGISTER";

constexpr quint16 NOTIFICATION_ID = 1;

void test_authorizationnotifier::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
}

void test_authorizationnotifier::initTestCase_data()
{
    QTest::addColumn<QString>("serviceNameForAlternateDevice");
    QTest::newRow("MT310s2") << QString("mt310s2d");
    QTest::newRow("MT581s2") << QString("mt581s2d");
}

void test_authorizationnotifier::init()
{
    QFETCH_GLOBAL(QString, serviceNameForAlternateDevice);
    SettingsContainer::TServiceConfig config = SettingsContainer::getServiceConfig(serviceNameForAlternateDevice);

    static ServerParams params {"foo", "0", QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + config.xsdFileName, QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + config.xmlFileName};
    AbstractFactoryI2cCtrlPtr ctrlFactory = std::make_shared<TestFactoryI2cCtrl>(false);

    m_PermissionCtrl = ctrlFactory->getPermissionCheckController();
    m_pcbServerTest = std::make_unique<TestPcbServerNotifications>(
        std::make_unique<SettingsContainer>(params),
        ctrlFactory, VeinTcp::MockTcpNetworkFactory::create());
    m_adjustmentStatusNull = new TestAdjustmentStatusInterfaceNull();
    m_pcbServerTest->insertScpiConnection(new ServiceStatusInterface(m_pcbServerTest->getSCPIInterface(),
                                                                     m_adjustmentStatusNull, ctrlFactory));
    m_pcbServerTest->initTestSCPIConnections();
}

void test_authorizationnotifier::cleanup()
{
    m_pcbServerTest = nullptr;
    delete m_adjustmentStatusNull;
}

QString test_authorizationnotifier::getAuthoStatus()
{
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, false, QByteArray(), 0, statusAuthorizationCommand);

    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(statusAuthorizationCommand);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    return protoCmd->m_sOutput;
}

void test_authorizationnotifier::findPCBServerScpiObject()
{
    cSCPIObject *scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(registerNotifierCommand);
    QVERIFY(scpiObject);
}

void test_authorizationnotifier::findStatusInterfaceScpiObject()
{
    cSCPIObject* scpiObject =  m_pcbServerTest->getSCPIInterface()->getSCPIObject(statusAuthorizationCommand);
    QVERIFY(scpiObject);
}

void test_authorizationnotifier::getInitialAuthoStatus()
{
     QCOMPARE(getAuthoStatus(), "0");
}

void test_authorizationnotifier::getNotiferId()
{
    QString scpiAuthorizationQuery = QString("%1 %2;%3;").arg(registerNotifierCommand).arg(statusAuthorizationCommand).arg(NOTIFICATION_ID);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, false, QByteArray(), 0, scpiAuthorizationQuery);

    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(registerNotifierCommand);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    QSignalSpy spy(m_pcbServerTest.get(), &TestPcbServerNotifications::notificationSent);
    QVERIFY(scpiDelegate->executeSCPI(protoCmd));

    QCOMPARE(spy.count(), 1);
    qint16 result = qvariant_cast<qint16>(spy.at(0).at(0));
    QCOMPARE(result, NOTIFICATION_ID);
}

void test_authorizationnotifier::notifyAuthoStatusEnabled()
{
    m_pcbServerTest->registerNotifier(statusAuthorizationCommand, NOTIFICATION_ID);
    QSignalSpy spy(m_pcbServerTest.get(), &TestPcbServerNotifications::notificationSent);
    
    static_cast<TestI2cCtrlEepromPermission*>(m_PermissionCtrl.get())->accessEnableAfter(100);
    TimeMachineForTest::getInstance()->processTimers(1500);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(getAuthoStatus(), "1");
}

void test_authorizationnotifier::notifyAuthoStatusEnabledDisabled()
{
    m_pcbServerTest->registerNotifier(statusAuthorizationCommand, NOTIFICATION_ID);
    QSignalSpy spy(m_pcbServerTest.get(), &TestPcbServerNotifications::notificationSent);
    
    static_cast<TestI2cCtrlEepromPermission*>(m_PermissionCtrl.get())->accessEnableAfter(100);
    TimeMachineForTest::getInstance()->processTimers(1500);
    QCOMPARE(spy.count(), 1);
    static_cast<TestI2cCtrlEepromPermission*>(m_PermissionCtrl.get())->accessDisableAfter(100);
    TimeMachineForTest::getInstance()->processTimers(1500);
    QCOMPARE(spy.count(), 2);

    QCOMPARE(getAuthoStatus(), "0");
}

void test_authorizationnotifier::unregisteredNotifierAuthoStatusEnabled()
{
    m_pcbServerTest->registerNotifier(statusAuthorizationCommand, NOTIFICATION_ID);
    m_pcbServerTest->unregisterNotifier();
    QSignalSpy spy(m_pcbServerTest.get(), &TestPcbServerNotifications::notificationSent);
    static_cast<TestI2cCtrlEepromPermission*>(m_PermissionCtrl.get())->accessEnableAfter(10);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 0);
}

void test_authorizationnotifier::noNotificationAuthoStatusEnabled()
{
    QSignalSpy spy(m_pcbServerTest.get(), &TestPcbServerNotifications::notificationSent);
    QCOMPARE(getAuthoStatus(), "0");
    static_cast<TestI2cCtrlEepromPermission*>(m_PermissionCtrl.get())->accessEnableAfter(10);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(getAuthoStatus(), "1");
}
