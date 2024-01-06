#ifndef SENSERANGE_H
#define SENSERANGE_H

#include "rangeadjustmentinterface.h"
#include "senserangecommon.h"

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
                      RangeAdjustmentInterface* justdata);
    ~Mt310s2SenseRange();
    void initSCPIConnection(QString leadingNodes) override;
    quint8 getAdjustmentStatus();

    RangeAdjustmentInterface* getJustData();

    void initJustData();
    void computeJustData();

    void setMMode(int mode);

private:
    RangeAdjustmentInterface* m_pJustdata;
};

#endif // SENSERANGE_H
