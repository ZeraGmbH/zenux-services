#include "statusinterface.h"
#include "accumulatorinterface.h"
#include "test_serverunregisternotifier.h"
#include "atmelsyscntrltest.h"
#include <QTest>

QTEST_MAIN(test_serverunregisternotifier)

static const char *registerNotifierCommand = "SERVER:REGISTER";
static const char *unregisterNotifierCommand = "SERVER:UNREGISTER";
static const char *statusAuthorizationCommand = "STATUS:AUTHORIZATION?";
static const char *accumulatorStatusCommand ="SYSTEM:ACCUMULATOR:STATUS?";

constexpr quint16 NOTIFICATION_ID = 1;

void test_serverunregisternotifier::init()
{
    cSCPI *scpiInterface = new cSCPI("foo");
    m_atmel = new MockAtmel();
    m_pcbServerTest = std::make_unique<PCBTestServer>("foo", "0", scpiInterface, m_atmel);
}

void test_serverunregisternotifier::cleanup()
{
    if(m_atmel) delete m_atmel;
    if(m_adjustmentStatusNull) delete m_adjustmentStatusNull;
}

cSCPIDelegate *test_serverunregisternotifier::getDelegate(QString cmd)
{
    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(cmd);
    return static_cast<cSCPIDelegate*>(scpiObject);
}

void test_serverunregisternotifier::oneScpiConnection()
{
    m_adjustmentStatusNull = new AdjustmentStatusNull();
    m_pcbServerTest->insertScpiConnection(new cStatusInterface(m_pcbServerTest->getSCPIInterface(), m_adjustmentStatusNull));
    m_pcbServerTest->initTestSCPIConnections();
    m_pcbServerTest->registerNotifier(statusAuthorizationCommand, NOTIFICATION_ID);
    QCOMPARE(getDelegate(statusAuthorizationCommand)->getTotalSubscribers(), 1);
    m_pcbServerTest->unregisterNotifier();
    QCOMPARE(getDelegate(statusAuthorizationCommand)->getTotalSubscribers(), 0);
}

void test_serverunregisternotifier::twoScpiConnections()
{
    m_adjustmentStatusNull = new AdjustmentStatusNull();
    m_pcbServerTest->insertScpiConnection(new cStatusInterface(m_pcbServerTest->getSCPIInterface(), m_adjustmentStatusNull));

    AtmelSysCntrlTest atmelSysCtrl("", 0, 0);
    m_pcbServerTest->insertScpiConnection(new AccumulatorInterface(m_pcbServerTest->getSCPIInterface(), &atmelSysCtrl));

    m_pcbServerTest->initTestSCPIConnections();
    m_pcbServerTest->registerNotifier(statusAuthorizationCommand, NOTIFICATION_ID);
    m_pcbServerTest->registerNotifier(accumulatorStatusCommand, NOTIFICATION_ID);
    QCOMPARE(getDelegate(statusAuthorizationCommand)->getTotalSubscribers(), 1);
    QCOMPARE(getDelegate(accumulatorStatusCommand)->getTotalSubscribers(), 1);
    m_pcbServerTest->unregisterNotifier();
    QCOMPARE(getDelegate(statusAuthorizationCommand)->getTotalSubscribers(), 0);
    QCOMPARE(getDelegate(accumulatorStatusCommand)->getTotalSubscribers(), 0);
}

//SERV:REG AUTHO:STAT?
//SERV:UNRE

