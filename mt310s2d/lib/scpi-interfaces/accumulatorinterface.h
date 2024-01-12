#ifndef ACCUMULATORINTERFACE_H
#define ACCUMULATORINTERFACE_H

#include "atmelctrlfactoryinterface.h"
#include "atmelctrlsystem.h"
#include "timertemplateqt.h"
#include "accumulatorsettings.h"
#include <scpiconnection.h>

enum accumulatorCommands{
    cmdStatus,
    cmdAccuStateOfCharge
};

class AccumulatorInterface : public ScpiConnection
{
public:
    AccumulatorInterface(cSCPI* scpiInterface, AccumulatorSettings* settings, AtmelCtrlFactoryInterfacePrt ctrlFactory);
    void initSCPIConnection(QString leadingNodes) override;
    QString getAccumulatorStatus();
    QString getAccuStateOfCharge();
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    NotificationString m_accumulatorStatus;
    NotificationString m_accuStateOfCharge;
    AtmelCtrlFactoryInterfacePrt m_ctrlFactory;
    TimerTemplateQtPtr m_pollingTimer;
};

#endif // ACCUMULATORINTERFACE_H
