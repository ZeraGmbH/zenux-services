#include "test_authorizationnotifier.h"
#include <scpisingletonfactory.h>
#include <QTest>

QTEST_MAIN(test_authorizationnotifier);

void test_authorizationnotifier::init()
{
    cSCPI *scpiInterface = new cSCPI("foo");
    m_pcbServerTest = std::make_unique<PCBTestServer>("foo", "0", scpiInterface);
}

void test_authorizationnotifier::findPCBServerScpiObject()
{
    cSCPI *m_scpiInterface = m_pcbServerTest->getSCPIInterface();
    cSCPIObject *scpiObject = m_scpiInterface->getSCPIObject("SERVER:REGISTER");
    QVERIFY(scpiObject);
}

void test_authorizationnotifier::findStatusInterfaceScpiObject()
{
    QString statusAuthorizationCommand = QString("STATUS:AUTHORIZATION?");
    cSCPIObject* scpiObject =  m_pcbServerTest->getSCPIInterface()->getSCPIObject("STATUS:AUTHORIZATION?");
    QVERIFY(scpiObject);
}

void test_authorizationnotifier::executeAuthorizationQuery()
{
     QString statusAuthorizationCommand = QString("STATUS:AUTHORIZATION?");
     cProtonetCommand* protoCmd = new cProtonetCommand(0, false, false, QByteArray(), 0, statusAuthorizationCommand);

     cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(statusAuthorizationCommand);
     cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
     QVERIFY(scpiDelegate->executeSCPI(protoCmd));
     QCOMPARE(protoCmd->m_sOutput, "1");
}

void test_authorizationnotifier::executeRegisterNotifier()
{
    QString registerNotifierCommand = QString("SERVER:REGISTER");
    QString statusAuthorizationCommand = QString("STATUS:AUTHORIZATION?");
    quint16 notifierId = 1;
    QString scpiAuthorizationQuery = QString("%1 %2;%3;").arg(registerNotifierCommand).arg(statusAuthorizationCommand).arg(notifierId);
    cProtonetCommand* protoCmd = new cProtonetCommand(nullptr, false, false, QByteArray(), 0, scpiAuthorizationQuery);

    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(registerNotifierCommand);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    QVERIFY(scpiDelegate->executeSCPI(protoCmd));
}
