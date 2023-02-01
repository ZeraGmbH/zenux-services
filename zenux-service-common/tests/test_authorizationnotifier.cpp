#include "test_authorizationnotifier.h"
#include <scpisingletonfactory.h>
#include <QTest>

QTEST_MAIN(test_authorizationnotifier);

void test_authorizationnotifier::init()
{
    cSCPI *scpiInterface = new cSCPI("foo");
    m_pcbServerTest = std::make_unique<PCBTestServer>("foo", "0", scpiInterface);
}

void test_authorizationnotifier::findScpiObject()
{
    cSCPI *m_scpiInterface = m_pcbServerTest->getSCPIInterface();
    cSCPIObject *scpiObject = m_scpiInterface->getSCPIObject("SERVER:REGISTER");
    QVERIFY(scpiObject);
}

void test_authorizationnotifier::scpiEmptyNotifier()
{
    QString registerNotifierCommand = QString("SERVER:REGISTER");
    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(registerNotifierCommand);
    QVERIFY(scpiObject);

    QString statusAuthorizationCommand = QString("STATUS:AUTHORIZATION?");
    QString scpiAuthorizationQuery = QString("%1 %2;%3;").arg(registerNotifierCommand).arg(statusAuthorizationCommand).arg(1);
    cProtonetCommand* protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiAuthorizationQuery);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    //QVERIFY(!scpiDelegate->executeSCPI(protoCmd));
}
