#ifndef ASYNCHNOTIFICATIONSINTERFACETEST_H
#define ASYNCHNOTIFICATIONSINTERFACETEST_H

#include <asynchnotificationsinterface.h>

class AsynchNotificationsInterfaceTest : public AsynchNotificationsInterface
{
public:
    AsynchNotificationsInterfaceTest(cSCPI *scpiInterface);
    void removeSCPIConnectionsTest();
    int getDelegateCount() const;
};

#endif // ASYNCHNOTIFICATIONSINTERFACETEST_H
