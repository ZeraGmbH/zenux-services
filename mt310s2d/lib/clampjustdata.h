#ifndef CLAMPJUSTDATA
#define CLAMPJUSTDATA

#include "rangeadjustmentinterface.h"
#include "mt310s2senserange.h"
#include "permissionfunctions.h"

// a clamp range consists of 2 stages . 1st the clamp itself and 2nd a voltage input range
// assigned to the clamp. so we need an interface for the clamp adjustment data (same as
// a normal senserange) but the adjustment data is the combination of the clamp's and the
// the voltage input range's adjustment data

class cSCPI;

class cClampJustData: public RangeAdjustmentInterface
{
    Q_OBJECT

public:
    cClampJustData(cSCPI* scpiinterface,
                   Mt310s2SenseRange* cascadedRange,
                   double cvRatio,
                   PermissionStructAdj permission = PermissionStructAdj());

protected:
    virtual double getGainCorrectionTotal(double par) override;
    virtual double getPhaseCorrectionTotal(double par) override;
    virtual double getOffsetCorrectionTotal(double par) override;

private:
    Mt310s2SenseRange* m_pFirstStageRange; //
    double m_cvRatio;
};

#endif // CLAMPJUSTDATA

