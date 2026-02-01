#ifndef SERVICESTATUSINTERFACE_H
#define SERVICESTATUSINTERFACE_H

#include "scpiconnection.h"
#include "abstractadjstatus.h"
#include "abstractfactoryi2cctrl.h"
#include "timertemplateqt.h"
#include <scpi.h>

class ServiceStatusInterface: public ScpiConnection
{
    Q_OBJECT
public:
    ServiceStatusInterface(std::shared_ptr<cSCPI> scpiInterface,
                           AbstractAdjStatus *adjustmentStatusInterface,
                           AbstractFactoryI2cCtrlPtr ctrlFactory);
    virtual void initSCPIConnection(const QString &leadingNodes) override;
protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
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

#endif // SERVICESTATUSINTERFACE_H
