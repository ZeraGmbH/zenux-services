#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include <QObject>
#include <QList>
#include <scpi.h>

#include "com5003d.h"
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


class cAdjustment;


class cStatusInterface: public cSCPIConnection
{
    Q_OBJECT

public:
    cStatusInterface(cCOM5003dServer *server);
    virtual void initSCPIConnection(QString leadingNodes);

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

private:
    cCOM5003dServer* m_pMyServer;
    quint8 getDeviceStatus();
    quint8 getAuthorizationStatus();
};

#endif // STATUSINTERFACE_H
