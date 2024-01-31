#ifndef ACCUMULATORINTERFACE_H
#define ACCUMULATORINTERFACE_H

#include "abstractfactoryi2cctrl.h"
#include "timertemplateqt.h"
#include "accumulatorsettings.h"
#include <scpiconnection.h>

class AccumulatorInterface : public ScpiConnection
{
public:
    AccumulatorInterface(cSCPI* scpiInterface, AccumulatorSettings* settings, AbstractFactoryI2cCtrlPtr ctrlFactory);
    void initSCPIConnection(QString leadingNodes) override;
private:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    QString getAccumulatorStatus();
    QString getAccuStateOfCharge();

    NotificationString m_accumulatorStatus;
    NotificationString m_accuStateOfCharge;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    TimerTemplateQtPtr m_pollingTimer;
};

#endif // ACCUMULATORINTERFACE_H
