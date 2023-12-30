#ifndef SENSERANGE_H
#define SENSERANGE_H

#include "senserangecommon.h"
#include "mt310s2justdata.h"

class Mt310s2SenseRange: public SenseRangeCommon
{
    Q_OBJECT
public:
    Mt310s2SenseRange(cSCPI* scpiinterface,
                      QString name,
                      bool avail,
                      double rValue,
                      double rejection,
                      double ovrejection,
                      quint8 rselcode,
                      quint16 mmask,
                      Mt310s2JustRangeTripletOffsetGainPhase* justdata);
    ~Mt310s2SenseRange();
    void initSCPIConnection(QString leadingNodes) override;
    quint8 getAdjustmentStatus();

    Mt310s2JustRangeTripletOffsetGainPhase* getJustData();

    void initJustData();
    void computeJustData();

    void setMMode(int mode);

private:
    Mt310s2JustRangeTripletOffsetGainPhase* m_pJustdata;
};

#endif // SENSERANGE_H
