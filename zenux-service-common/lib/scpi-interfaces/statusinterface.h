#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include "scpiconnection.h"
#include "adjustmentstatusinterface.h"
#include <scpi.h>

class cStatusInterface: public ScpiConnection
{
    Q_OBJECT
public:
    cStatusInterface(cSCPI *scpiInterface, AdjustmentStatusInterface *adjustmentStatusInterface);
    virtual void initSCPIConnection(QString leadingNodes) override;
protected:
    void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString getDeviceStatus();
    QString getAuthorizationStatus();
    AdjustmentStatusInterface *m_adjustmentStatusInterface;
    NotificationString m_notifierAutorization;
};

#endif // STATUSINTERFACE_H
