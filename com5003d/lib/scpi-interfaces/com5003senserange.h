#ifndef SENSERANGE_H
#define SENSERANGE_H

#include "senserangecommon.h"
#include "rangeadjustmentinterface.h"

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

    RangeAdjustmentInterface *getJustData();

    void initJustData();
    void computeJustData();

private:
    Atmel* m_pATMEL;
    RangeAdjustmentInterface* m_pJustdata;
};

#endif // SENSERANGE_H
