#ifndef ACCUMULATORINTERFACE_H
#define ACCUMULATORINTERFACE_H

#include "atmelsysctrl.h"
#include <scpiconnection.h>

enum accumulatorCommands{
    cmdStatus,
    cmdSoc
};

class AccumulatorInterface : public ScpiConnection
{
public:
    AccumulatorInterface(cSCPI* scpiInterface, cATMELSysCtrl *atmelSysCntrl);
    void initSCPIConnection(QString leadingNodes) override;
    QString getAccumulatorStatus();
    QString getAccumulatorSoc();
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    NotificationString m_accumulatorStatus;
    NotificationString m_accumulatorSoc;
    cATMELSysCtrl *m_atmelSysCntrl;
};

#endif // ACCUMULATORINTERFACE_H
