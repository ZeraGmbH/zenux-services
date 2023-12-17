#ifndef SENSERANGECOMMON_H
#define SENSERANGECOMMON_H

#include "scpiconnection.h"
#include "adjustmentstatusinterface.h"

class SenseRangeCommon : public ScpiConnection, public AdjustmentStatusInterface
{
    Q_OBJECT
public:
    SenseRangeCommon(cSCPI* scpiInterface,
                     QString name,
                     QString alias,
                     bool avail,
                     double rValue,
                     double rejection,
                     double ovrejection,
                     double adcrejection,
                     quint8 rselcode);
protected:
    QString m_sName; // the range name
    QString m_sAlias; // the range alias name
    bool m_bAvail; // range io avail or not
    double m_fRValue; // upper range value
    double m_fRejection; // 100% rejection value
    double m_fOVRejection; // overload rejection value
    double m_fADCRejection; // the adc's maximum rejection
    quint8 m_nSelCode; // selection code
};

#endif // SENSERANGECOMMON_H
