#include "test_authorizationnotifier.h"
#include <scpisingletonfactory.h>
#include <QTest>
#include <QSignalSpy>

QTEST_MAIN(test_authorizationnotifier);

static const char *statusAuthorizationCommand = "STATUS:AUTHORIZATION?";
static const char *registerNotifierCommand = "SERVER:REGISTER";

void test_authorizationnotifier::init()
{
    cSCPI *scpiInterface = new cSCPI("foo");
    m_atmel = new MockAtmel();
    m_pcbServerTest = std::make_unique<PCBTestServer>("foo", "0", scpiInterface, m_atmel);
}

void test_authorizationnotifier::cleanup()
{
    delete m_atmel;
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
     cProtonetCommand* protoCmd = new cProtonetCommand(0, false, false, QByteArray(), 0, statusAuthorizationCommand);

     cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(statusAuthorizationCommand);
     cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
     QVERIFY(scpiDelegate->executeSCPI(protoCmd));
     QCOMPARE(protoCmd->m_sOutput, "0");
}

void test_authorizationnotifier::getNotiferId()
{
    quint16 notifierId = 1;
    QString scpiAuthorizationQuery = QString("%1 %2;%3;").arg(registerNotifierCommand).arg(statusAuthorizationCommand).arg(notifierId);
    cProtonetCommand* protoCmd = new cProtonetCommand(nullptr, false, false, QByteArray(), 0, scpiAuthorizationQuery);

    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(registerNotifierCommand);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    QSignalSpy spy(m_pcbServerTest.get(), &PCBTestServer::notificationSent);
    QVERIFY(scpiDelegate->executeSCPI(protoCmd));

    QCOMPARE(spy.count(), 1);
    qint16 result = qvariant_cast<qint16>(spy.at(0).at(0));
    QCOMPARE(result, notifierId);
}

void test_authorizationnotifier::authoStatusChangedToOne()
{
    quint16 notifierId = 1;
    QString scpiAuthorizationQuery = QString("%1 %2;%3;").arg(registerNotifierCommand).arg(statusAuthorizationCommand).arg(notifierId);
    cProtonetCommand* protoCmd = new cProtonetCommand(nullptr, false, false, QByteArray(), 0, scpiAuthorizationQuery);
    //register notification
    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(registerNotifierCommand);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    QSignalSpy spy(m_pcbServerTest.get(), &PCBTestServer::notificationSent);
    QVERIFY(scpiDelegate->executeSCPI(protoCmd));

    m_atmel->accessEnableAfter(100);
    //catch the notification
    QVERIFY(spy.wait(1500));

    //read status
    protoCmd = new cProtonetCommand(0, false, false, QByteArray(), 0, statusAuthorizationCommand);
    scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(statusAuthorizationCommand);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QString new_status = protoCmd->m_sOutput;
    QCOMPARE(new_status, "1");
}

void test_authorizationnotifier::authoStatusChangedToZero()
{
    quint16 notifierId = 1;
    QString scpiAuthorizationQuery = QString("%1 %2;%3;").arg(registerNotifierCommand).arg(statusAuthorizationCommand).arg(notifierId);
    cProtonetCommand* protoCmd = new cProtonetCommand(nullptr, false, false, QByteArray(), 0, scpiAuthorizationQuery);
    //register notification
    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(registerNotifierCommand);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    QSignalSpy spy(m_pcbServerTest.get(), &PCBTestServer::notificationSent);
    QVERIFY(scpiDelegate->executeSCPI(protoCmd));

    m_atmel->accessEnableAfter(100);
    QVERIFY(spy.wait(1500));
    m_atmel->accessDisableAfter(100);
    QVERIFY(spy.wait(1500));

    protoCmd = new cProtonetCommand(0, false, false, QByteArray(), 0, statusAuthorizationCommand);
    scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(statusAuthorizationCommand);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QString new_status = protoCmd->m_sOutput;
    QCOMPARE(new_status, "0");
}
