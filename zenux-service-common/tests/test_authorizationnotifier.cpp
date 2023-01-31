#include "test_authorizationnotifier.h"
#include <QTest>

QTEST_MAIN(test_authorizationnotifier);

void test_authorizationnotifier::init()
{
    PCBServerTest = std::make_unique<PCBTestServer>("foo", "0");
    PCBServerTest->initSCPIConnection("");
}

void test_authorizationnotifier::findScpiObject()
{
    cSCPI *m_scpiInterface = PCBServerTest->getSCPIInterface();
    cSCPIObject *scpiObject = m_scpiInterface->getSCPIObject("SERVER:REGISTER");
    QVERIFY(scpiObject);
}

void test_authorizationnotifier::scpiEmptyNotifier()
{
    QString registerNotifierCommand = QString("SERVER:REGISTER");
    cSCPIObject* scpiObject = PCBServerTest->getSCPIInterface()->getSCPIObject(registerNotifierCommand);
    QVERIFY(scpiObject);

    QString statusAuthorizationCommand = QString("STATUS:AUTHORIZATION?");
    QString scpiAuthorizationQuery = QString("%1 %2;%3;").arg(registerNotifierCommand).arg(statusAuthorizationCommand).arg(1);
    cProtonetCommand* protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiAuthorizationQuery);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    //QVERIFY(!scpiDelegate->executeSCPI(protoCmd));
}
