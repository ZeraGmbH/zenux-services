#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include <QObject>
#include <QList>
#include <scpi.h>

#include "mt310s2d.h"
#include <scpiconnection.h>
#include <scpidelegate.h>

namespace StatusSystem
{

enum StatusCommands
{
    cmdDevice,
    cmdAdjustment,
    cmdAuthorization
};
}

class cStatusInterface: public cSCPIConnection
{
    Q_OBJECT

public:
    cStatusInterface(cMT310S2dServer *server);
    virtual void initSCPIConnection(QString leadingNodes);

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

private:
    cMT310S2dServer* m_pMyServer;
    quint8 getDeviceStatus();
    quint8 getAuthorizationStatus();
};

#endif // STATUSINTERFACE_H
