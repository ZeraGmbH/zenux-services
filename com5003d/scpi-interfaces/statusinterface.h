#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include "com5003d.h"
#include "scpiconnection.h"
#include <QObject>
#include <QList>

namespace StatusSystem
{

enum StatusCommands
{
    cmdDevice,
    cmdAdjustment,
    cmdAuthorization
};
}

class cStatusInterface: public ScpiConnection
{
    Q_OBJECT

public:
    cStatusInterface(cCOM5003dServer *server);
    virtual void initSCPIConnection(QString leadingNodes) override;

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    cCOM5003dServer* m_pMyServer;
    quint8 getDeviceStatus();
    quint8 getAuthorizationStatus();
};

#endif // STATUSINTERFACE_H
