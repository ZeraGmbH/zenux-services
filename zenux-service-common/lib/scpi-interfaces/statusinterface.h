#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include "scpiconnection.h"
#include "abstractadjstatus.h"
#include "abstractfactoryi2cctrl.h"
#include "timertemplateqt.h"
#include <scpi.h>

class cStatusInterface: public ScpiConnection
{
    Q_OBJECT
public:
    cStatusInterface(cSCPI *scpiInterface, AbstractAdjStatus *adjustmentStatusInterface, AbstractFactoryI2cCtrlPtr ctrlFactory);
    virtual void initSCPIConnection(QString leadingNodes) override;
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString getControllerAvail();
    QString getAuthorizationStatus();
    AbstractAdjStatus *m_adjustmentStatusInterface;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    NotificationString m_notifierAutorization;
    TimerTemplateQtPtr m_periodicTimer;
private slots:
    void onNotifierRegistered(NotificationString* notifier) override;
    void onNotifierUnregistered();
};

#endif // STATUSINTERFACE_H
