#ifndef SENSERANGE_H
#define SENSERANGE_H

#include "senserangecommon.h"
#include "com5003justdata.h"

class Atmel;

class Com5003SenseRange : public SenseRangeCommon
{
    Q_OBJECT
public:
    Com5003SenseRange(cSCPI *scpiinterface,
                QString name,
                bool avail,
                double rValue,
                double rejection,
                double ovrejection,
                quint8 rselcode);
    ~Com5003SenseRange();
    void initSCPIConnection(QString leadingNodes) override;
    quint8 getAdjustmentStatus();

    Com5003JustRangeTripletOffsetGainPhase* getJustData();

    void initJustData();
    void computeJustData();

private:
    Atmel* m_pATMEL;
    Com5003JustRangeTripletOffsetGainPhase* m_pJustdata;
};

#endif // SENSERANGE_H
