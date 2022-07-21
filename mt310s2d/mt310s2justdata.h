// header datei mt310s2justdata.h

#ifndef MT310S2JUSTDATA_H
#define MT310S2JUSTDATA_H

#include <QObject>
#include <scpiconnection.h>

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
class cJustData;


class cMT310S2JustData: public cSCPIConnection  // alle korrekturdaten für einen bereich + status
{
    Q_OBJECT

public:
    cMT310S2JustData(cSCPI* scpiinterface);
    ~cMT310S2JustData();
    virtual void initSCPIConnection(QString leadingNodes);

    cJustData* m_pGainCorrection;
    cJustData* m_pPhaseCorrection; 
    cJustData* m_pOffsetCorrection;
    
    void Serialize(QDataStream&); // zum schreiben aller justagedaten in flashspeicher
    void Deserialize(QDataStream&); // zum lesen aller justagedaten aus flashspeicher
    quint8 getAdjustmentStatus();
    void initJustData();
    void computeJustData();

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

protected:
    QString mReadGainCorrection(QString&sInput);
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


#endif // MT310S2JUSTDATA_H

