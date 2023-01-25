#include "asynchnotificationsinterfacetest.h"

AsynchNotificationsInterfaceTest::AsynchNotificationsInterfaceTest(cSCPI* scpiInterface) :
    AsynchNotificationsInterface(scpiInterface)
{

}

void AsynchNotificationsInterfaceTest::removeSCPIConnectionsTest()
{
    removeSCPIConnections();
}

int AsynchNotificationsInterfaceTest::getDelegateCount() const
{
    return m_DelegateList.count();
}
