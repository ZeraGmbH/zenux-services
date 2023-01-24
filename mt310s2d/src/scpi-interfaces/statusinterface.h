#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include "scpiconnection.h"
#include "adjustmentstatusinterface.h"
#include <scpi.h>

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
    cStatusInterface(cSCPI *scpiInterface, AdjustmentStatusInterface *adjustmentStatusInterface);
    virtual void initSCPIConnection(QString leadingNodes) override;

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    quint8 getDeviceStatus();
    quint8 getAuthorizationStatus();
    AdjustmentStatusInterface *m_adjustmentStatusInterface;
};

#endif // STATUSINTERFACE_H
