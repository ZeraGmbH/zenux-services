#ifndef ACCUMULATORINTERFACE_H
#define ACCUMULATORINTERFACE_H

#include "atmelsysctrl.h"
#include "timerperiodicqt.h"
#include "accumulatorsettings.h"
#include <scpiconnection.h>

enum accumulatorCommands{
    cmdStatus,
    cmdAccuStateOfCharge
};

class AccumulatorInterface : public ScpiConnection
{
public:
    AccumulatorInterface(cSCPI* scpiInterface, cATMELSysCtrl *atmelSysCntrl, AccumulatorSettings* settings);
    void initSCPIConnection(QString leadingNodes) override;
    QString getAccumulatorStatus();
    QString getAccuStateOfCharge();
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    NotificationString m_accumulatorStatus;
    NotificationString m_accuStateOfCharge;
    cATMELSysCtrl *m_atmelSysCntrl;
    TimerTemplateQtPtr m_pollingTimer;
};

#endif // ACCUMULATORINTERFACE_H
