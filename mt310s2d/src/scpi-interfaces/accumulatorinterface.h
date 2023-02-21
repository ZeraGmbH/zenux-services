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
    quint16 m_status = 0;
    cATMELSysCtrl *m_atmelSysCntrl;
};

#endif // ACCUMULATORINTERFACE_H
