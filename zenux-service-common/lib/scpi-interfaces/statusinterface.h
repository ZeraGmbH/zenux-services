#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include "scpiconnection.h"
#include "adjustmentstatusinterface.h"
#include "timerperiodicqt.h"
#include <scpi.h>

class cStatusInterface: public ScpiConnection
{
    Q_OBJECT
public:
    cStatusInterface(cSCPI *scpiInterface, AdjustmentStatusInterface *adjustmentStatusInterface);
    virtual void initSCPIConnection(QString leadingNodes) override;
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString getDeviceStatus();
    QString getAuthorizationStatus();
    AdjustmentStatusInterface *m_adjustmentStatusInterface;
    NotificationString m_notifierAutorization;
    TimerTemplateQtPtr m_periodicTimer;
private slots:
    void onNotifierRegistered(NotificationString* notifier) override;
    void onNotifierUnregistered(NotificationString* notifier) override;
};

#endif // STATUSINTERFACE_H
