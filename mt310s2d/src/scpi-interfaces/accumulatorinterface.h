#ifndef ACCUMULATORINTERFACE_H
#define ACCUMULATORINTERFACE_H

#include "atmelsysctrl.h"
#include <scpiconnection.h>

enum accumulatorCommands{
    cmdStatus
};

class AccumulatorInterface : public ScpiConnection
{
public:
    AccumulatorInterface(cSCPI* scpiInterface, cATMELSysCtrl *atmelSysCntrl);
    void initSCPIConnection(QString leadingNodes) override;
    QString getAccumulatorStatus();
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    NotificationString m_accumulatorStatus;
    cATMELSysCtrl *m_atmelSysCntrl;
};

#endif // ACCUMULATORINTERFACE_H
