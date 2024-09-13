#ifndef ACCUMULATORINTERFACE_H
#define ACCUMULATORINTERFACE_H

#include "abstractfactoryi2cctrl.h"
#include "timertemplateqt.h"
#include "accumulatorsettings.h"
#include <scpiconnection.h>

class AccumulatorInterface : public ScpiConnection
{
    Q_OBJECT
public:
    AccumulatorInterface(cSCPI* scpiInterface, AccumulatorSettings* settings, AbstractFactoryI2cCtrlPtr ctrlFactory);
    void initSCPIConnection(QString leadingNodes) override;
signals:
    void sigAccumulatorStatusChange(uint8_t status);
private:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    void getAccumulatorStatus();
    void getAccuStateOfCharge();

    NotificationString m_accumulatorStatus;
    NotificationString m_accuStateOfCharge;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    TimerTemplateQtPtr m_pollingTimer;
    QString m_oldaccuStatus;
};

#endif // ACCUMULATORINTERFACE_H
