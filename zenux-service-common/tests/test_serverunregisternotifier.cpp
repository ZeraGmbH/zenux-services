#include "statusinterface.h"
#include "test_serverunregisternotifier.h"
#include <QTest>

QTEST_MAIN(test_serverunregisternotifier)

static const char *registerNotifierCommand = "SERVER:REGISTER";
static const char *unregisterNotifierCommand = "SERVER:UNREGISTER";
static const char *statusAuthorizationCommand = "STATUS:AUTHORIZATION?";

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

void test_serverunregisternotifier::registerNotifier(QString inputCmd)
{
    QString scpiAuthorizationQuery = QString("%1 %2;%3;").arg(registerNotifierCommand).arg(inputCmd).arg(NOTIFICATION_ID);
    cProtonetCommand* protoCmd = new cProtonetCommand(nullptr, false, false, QByteArray(), 0, scpiAuthorizationQuery);
    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(registerNotifierCommand);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
}

void test_serverunregisternotifier::unregisterNotifier()
{
    QString scpiAuthorizationQuery = QString("%1 %2;").arg(unregisterNotifierCommand).arg("");
    cProtonetCommand* protoCmd = new cProtonetCommand(nullptr, false, false, QByteArray(), 0, scpiAuthorizationQuery);
    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(unregisterNotifierCommand);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
}

void test_serverunregisternotifier::oneScpiConnection()
{
    m_adjustmentStatusNull = new AdjustmentStatusNull();
    m_pcbServerTest->insertScpiConnection(new cStatusInterface(m_pcbServerTest->getSCPIInterface(), m_adjustmentStatusNull));
    m_pcbServerTest->initTestSCPIConnections();
    registerNotifier(statusAuthorizationCommand);
    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(statusAuthorizationCommand);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    QCOMPARE(scpiDelegate->getTotalSubscribers(), 1);
    unregisterNotifier();
    QCOMPARE(scpiDelegate->getTotalSubscribers(), 0);
}

