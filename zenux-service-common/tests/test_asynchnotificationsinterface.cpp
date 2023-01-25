#include "test_asynchnotificationsinterface.h"
#include "asynchnotificationsinterface.h"
#include "scpidelegate.h"
#include <QTest>

QTEST_MAIN(test_asynchnotificationsinterface);

void test_asynchnotificationsinterface::init()
{
    m_scpiInterface = std::make_unique<cSCPI>("foo");
}

void test_asynchnotificationsinterface::findScpiRegisterObject()
{
    AsynchNotificationsInterface testInterface(m_scpiInterface.get());
    testInterface.initSCPIConnection("");
    cSCPIObject *scpiObject = m_scpiInterface->getSCPIObject("SERVER:REGISTER");
    QVERIFY(scpiObject);
}

void test_asynchnotificationsinterface::findScpiUnregisterObject()
{
    AsynchNotificationsInterface testInterface(m_scpiInterface.get());
    testInterface.initSCPIConnection("");
    cSCPIObject *scpiObject = m_scpiInterface->getSCPIObject("SERVER:UNREGISTER");
    QVERIFY(scpiObject);
}
