#ifndef ASYNCHNOTIFICATIONSINTERFACE_H
#define ASYNCHNOTIFICATIONSINTERFACE_H

#include <scpiconnection.h>

class AsynchNotificationsInterface : public ScpiConnection
{
public:
    AsynchNotificationsInterface(cSCPI* scpiInterface);

    void initSCPIConnection(QString leadingNodes) override;
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
};

#endif // ASYNCHNOTIFICATIONSINTERFACE_H
