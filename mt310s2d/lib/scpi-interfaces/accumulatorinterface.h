#ifndef ACCUMULATORINTERFACE_H
#define ACCUMULATORINTERFACE_H

#include "abstractfactoryi2cctrl.h"
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
    AccumulatorInterface(cSCPI* scpiInterface, AccumulatorSettings* settings, FactoryControllerAbstractPtr ctrlFactory);
    void initSCPIConnection(QString leadingNodes) override;
    QString getAccumulatorStatus();
    QString getAccuStateOfCharge();
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    NotificationString m_accumulatorStatus;
    NotificationString m_accuStateOfCharge;
    FactoryControllerAbstractPtr m_ctrlFactory;
    TimerTemplateQtPtr m_pollingTimer;
};

#endif // ACCUMULATORINTERFACE_H
