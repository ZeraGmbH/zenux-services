#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include "mt310s2d.h"
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
    cStatusInterface(cMT310S2dServer *server);
    virtual void initSCPIConnection(QString leadingNodes) override;

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    cMT310S2dServer* m_pMyServer;
    quint8 getDeviceStatus();
    quint8 getAuthorizationStatus();
};

#endif // STATUSINTERFACE_H
