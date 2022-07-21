// header datei com5003justdata.h

#ifndef COM5003JUSTDATA_H
#define COM5003JUSTDATA_H

#include <QObject>
#include <scpiconnection.h>

enum com5003JustCommands
{
    com5003Gain,
    com5003JustGain,
    com5003Phase,
    com5003JustPhase,
    com5003Offset,
    com5003JustOffset,
    com5003JustStatus,
    com5003JustCompute,
    com5003JustInit
};


const int GainCorrOrder = 3; // ax^3 + bx^2 + cx + d
const int PhaseCorrOrder  = 3;
const int OffsetCorrOrder = 3;


class QDataStream;
class cJustData;


class cCOM5003JustData: public cSCPIConnection  // alle korrekturdaten für einen bereich + status
{
    Q_OBJECT

public:
    cCOM5003JustData(cSCPI* scpiinterface);
    ~cCOM5003JustData();
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

private:    
    QString mReadGainCorrection(QString&sInput);
    QString mReadPhaseCorrection(QString&sInput);
    QString mReadOffsetCorrection(QString&sInput);
    QString m_ReadStatus(QString& sInput);
    QString m_ComputeJustData(QString& sInput);
    QString m_InitJustData(QString& sInput);
};


#endif

