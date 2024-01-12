#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include "scpiconnection.h"
#include "adjustmentstatusinterface.h"
#include "atmelctrlfactoryinterface.h"
#include "timertemplateqt.h"
#include <scpi.h>

class cStatusInterface: public ScpiConnection
{
    Q_OBJECT
public:
    cStatusInterface(cSCPI *scpiInterface, AdjustmentStatusInterface *adjustmentStatusInterface, AtmelCtrlFactoryInterfacePrt ctrlFactory);
    virtual void initSCPIConnection(QString leadingNodes) override;
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString getControllerAvail();
    QString getAuthorizationStatus();
    AdjustmentStatusInterface *m_adjustmentStatusInterface;
    AtmelCtrlFactoryInterfacePrt m_ctrlFactory;
    NotificationString m_notifierAutorization;
    TimerTemplateQtPtr m_periodicTimer;
private slots:
    void onNotifierRegistered(NotificationString* notifier) override;
    void onNotifierUnregistered();
};

#endif // STATUSINTERFACE_H
