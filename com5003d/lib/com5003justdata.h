#ifndef COM5003JUSTDATA_H
#define COM5003JUSTDATA_H

#include "scpiconnection.h"
#include "permissionfunctions.h"
#include "adjustmentstatusinterface.h"

enum DirectJustCommands
{
    DirectGain,
    DirectJustGain,
    DirectPhase,
    DirectJustPhase,
    DirectOffset,
    DirectJustOffset,
    DirectJustStatus,
    DirectJustCompute,
    DirectJustInit
};


const int GainCorrOrder = 3; // ax^3 + bx^2 + cx + d
const int PhaseCorrOrder  = 3;
const int OffsetCorrOrder = 3;


class QDataStream;
class JustDataInterface;


class Com5003JustRangeTripletOffsetGainPhase : public ScpiConnection, public AdjustmentStatusInterface // alle korrekturdaten für einen bereich + status
{
    Q_OBJECT

public:
    Com5003JustRangeTripletOffsetGainPhase(cSCPI* scpiinterface, PermissionStructAdj permissions = PermissionStructAdj());
    ~Com5003JustRangeTripletOffsetGainPhase();
    virtual void initSCPIConnection(QString leadingNodes) override;

    JustDataInterface* m_pGainCorrection;
    JustDataInterface* m_pPhaseCorrection; 
    JustDataInterface* m_pOffsetCorrection;
    
    void Serialize(QDataStream&); // zum schreiben aller justagedaten in flashspeicher
    void Deserialize(QDataStream&); // zum lesen aller justagedaten aus flashspeicher
    quint8 getAdjustmentStatus() override;
    void initJustData();
    void computeJustData();

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    double getJustGainCorrection(double par);
    double getJustPhaseCorrection(double par);
    double getJustOffsetCorrection(double par);
    virtual double getTotalGainCorrection(double par);
    virtual double getTotalPhaseCorrection(double par);
    virtual double getTotalOffsetCorrection(double par);

private:
    QString scpiQueryGainCorrection(const QString &scpiInput);
    QString scpiQueryJustGainCorrection(QString &scpiInput);
    QString scpiQueryPhaseCorrection(QString &scpiInput);
    QString scpiQueryJustPhaseCorrection(QString &scpiInput);
    QString scpiQueryOffsetCorrection(QString &scpiInput);
    QString scpiQueryJustOffsetCorrection(QString &scpiInput);
    QString scpiQueryStatus(QString &scpiInput);
    QString scpiCmdComputeJustData(QString &scpiInput);
    QString scpiCmdInitJustData(QString &scpiInput); // done in Adjustmentmodule - left for compatibility

private:
    PermissionStructAdj m_permissions;
};

#endif
