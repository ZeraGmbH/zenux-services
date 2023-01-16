#ifndef COM5003JUSTDATA_H
#define COM5003JUSTDATA_H

#include "scpiconnection.h"

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


class JustDataRangeGainPhaseOffset: public ScpiConnection  // alle korrekturdaten für einen bereich + status
{
    Q_OBJECT

public:
    JustDataRangeGainPhaseOffset(cSCPI* scpiinterface);
    ~JustDataRangeGainPhaseOffset();
    virtual void initSCPIConnection(QString leadingNodes) override;

    JustDataInterface* m_pGainCorrection;
    JustDataInterface* m_pPhaseCorrection; 
    JustDataInterface* m_pOffsetCorrection;
    
    void Serialize(QDataStream&); // zum schreiben aller justagedaten in flashspeicher
    void Deserialize(QDataStream&); // zum lesen aller justagedaten aus flashspeicher
    quint8 getAdjustmentStatus();
    void initJustData();
    void computeJustData();

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

protected:
    QString scpiGetGainCorrection(const QString &scpiInput);
    QString mReadJustGainCorrection(QString&sInput);
    QString mReadPhaseCorrection(QString&sInput);
    QString mReadJustPhaseCorrection(QString&sInput);
    QString mReadOffsetCorrection(QString&sInput);
    QString mReadJustOffsetCorrection(QString&sInput);
    QString m_ReadStatus(QString& sInput);
    QString m_ComputeJustData(QString& sInput);
    QString m_InitJustData(QString& sInput);

    virtual double getGainCorrection(double par);
    virtual double getJustGainCorrection(double par);
    virtual double getPhaseCorrection(double par);
    virtual double getJustPhaseCorrection(double par);
    virtual double getOffsetCorrection(double par);
    virtual double getJustOffsetCorrection(double par);
};

#endif
