#ifndef SERVICESTATUSINTERFACE_H
#define SERVICESTATUSINTERFACE_H

#include "scpiserverconnection.h"
#include "abstractadjstatus.h"
#include "abstractfactoryi2cctrl.h"
#include "timertemplateqt.h"
#include <scpi.h>

class ServiceStatusInterface: public ScpiServerConnection
{
    Q_OBJECT
public:
    ServiceStatusInterface(std::shared_ptr<cSCPI> scpiInterface,
                           AbstractAdjStatus *adjustmentStatusInterface,
                           AbstractFactoryI2cCtrlPtr ctrlFactory,
                           bool hasSourceGenerator);
    void initSCPIConnection() override;

private slots:
    void onNotifierRegistered(NotificationString* notifier) override;
    void onNotifierUnregistered();
private:
    void executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd) override;
    QString getControllerAvail();
    QString getAuthorizationStatus();

    const AbstractAdjStatus *m_adjustmentStatusInterface;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    NotificationString m_notifierAutorization;
    TimerTemplateQtPtr m_periodicTimer;
    const bool m_hasSourceGenerator;
};

#endif // SERVICESTATUSINTERFACE_H
