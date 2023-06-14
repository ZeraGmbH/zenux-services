#include "test_asynchnotificationsinterface.h"
#include "asynchnotificationsinterfacetest.h"
#include "asynchnotificationsinterface.h"
#include "scpidelegate.h"
#include <QTest>

QTEST_MAIN(test_asynchnotificationsinterface);

void test_asynchnotificationsinterface::init()
{
    m_scpiInterface = std::make_unique<cSCPI>();
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

void test_asynchnotificationsinterface::expectTwoDelegates()
{
    AsynchNotificationsInterfaceTest testInterface(m_scpiInterface.get());
    testInterface.initSCPIConnection("");
    QCOMPARE(testInterface.getDelegateCount(), 2);
}

void test_asynchnotificationsinterface::removeScpiConnection()
{
    AsynchNotificationsInterfaceTest testInterface(m_scpiInterface.get());
    testInterface.initSCPIConnection("");
    testInterface.removeSCPIConnectionsTest();
    QCOMPARE(testInterface.getDelegateCount(), 0);
}
