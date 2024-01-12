#include "test_authorizationnotifier.h"
#include "adjustmentstatusnull.h"
#include "statusinterface.h"
#include "mockatmelctrlfactory.h"
#include "atmelpermissionmock.h"
#include <scpisingletonfactory.h>
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
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

void test_authorizationnotifier::init()
{
    static ServerParams params {"foo", "0", QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xsd", QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xml"};
    cSCPI *scpiInterface = new cSCPI();
    AtmelCtrlFactoryInterfacePrt ctrlFactory = std::make_shared<MockAtmelCtrlFactory>(false);

    m_atmelPermissionPtrU = ctrlFactory->getPermissionCheckController();
    m_pcbServerTest = std::make_unique<PCBTestServer>(params, scpiInterface, ctrlFactory);
    m_adjustmentStatusNull = new AdjustmentStatusNull();
    m_pcbServerTest->insertScpiConnection(new cStatusInterface(m_pcbServerTest->getSCPIInterface(), m_adjustmentStatusNull, ctrlFactory));
    m_pcbServerTest->initTestSCPIConnections();
}

void test_authorizationnotifier::cleanup()
{
    m_pcbServerTest = nullptr;
    delete m_adjustmentStatusNull;
}

QString test_authorizationnotifier::getAuthoStatus()
{
    cProtonetCommand* protoCmd = new cProtonetCommand(0, false, false, QByteArray(), 0, statusAuthorizationCommand);

    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(statusAuthorizationCommand);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    return protoCmd->m_sOutput;
}

void test_authorizationnotifier::findPCBServerScpiObject()
{
    cSCPI *m_scpiInterface = m_pcbServerTest->getSCPIInterface();
    cSCPIObject *scpiObject = m_scpiInterface->getSCPIObject(registerNotifierCommand);
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
    cProtonetCommand* protoCmd = new cProtonetCommand(nullptr, false, false, QByteArray(), 0, scpiAuthorizationQuery);

    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(registerNotifierCommand);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    QSignalSpy spy(m_pcbServerTest.get(), &PCBTestServer::notificationSent);
    QVERIFY(scpiDelegate->executeSCPI(protoCmd));

    QCOMPARE(spy.count(), 1);
    qint16 result = qvariant_cast<qint16>(spy.at(0).at(0));
    QCOMPARE(result, NOTIFICATION_ID);
}

void test_authorizationnotifier::notifyAuthoStatusEnabled()
{
    m_pcbServerTest->registerNotifier(statusAuthorizationCommand, NOTIFICATION_ID);
    QSignalSpy spy(m_pcbServerTest.get(), &PCBTestServer::notificationSent);

    static_cast<AtmelPermissionMock*>(m_atmelPermissionPtrU.get())->accessEnableAfter(100);
    TimeMachineForTest::getInstance()->processTimers(1500);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(getAuthoStatus(), "1");
}

void test_authorizationnotifier::notifyAuthoStatusEnabledDisabled()
{
    m_pcbServerTest->registerNotifier(statusAuthorizationCommand, NOTIFICATION_ID);
    QSignalSpy spy(m_pcbServerTest.get(), &PCBTestServer::notificationSent);

    static_cast<AtmelPermissionMock*>(m_atmelPermissionPtrU.get())->accessEnableAfter(100);
    TimeMachineForTest::getInstance()->processTimers(1500);
    QCOMPARE(spy.count(), 1);
    static_cast<AtmelPermissionMock*>(m_atmelPermissionPtrU.get())->accessDisableAfter(100);
    TimeMachineForTest::getInstance()->processTimers(1500);
    QCOMPARE(spy.count(), 2);

    QCOMPARE(getAuthoStatus(), "0");
}

void test_authorizationnotifier::unregisteredNotifierAuthoStatusEnabled()
{
    m_pcbServerTest->registerNotifier(statusAuthorizationCommand, NOTIFICATION_ID);
    m_pcbServerTest->unregisterNotifier();
    QSignalSpy spy(m_pcbServerTest.get(), &PCBTestServer::notificationSent);
    static_cast<AtmelPermissionMock*>(m_atmelPermissionPtrU.get())->accessEnableAfter(10);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 0);
}

void test_authorizationnotifier::noNotificationAuthoStatusEnabled()
{
    QSignalSpy spy(m_pcbServerTest.get(), &PCBTestServer::notificationSent);
    QCOMPARE(getAuthoStatus(), "0");
    static_cast<AtmelPermissionMock*>(m_atmelPermissionPtrU.get())->accessEnableAfter(10);
    TimeMachineForTest::getInstance()->processTimers(1000);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(getAuthoStatus(), "1");
}
