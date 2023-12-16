#ifndef CLAMPJUSTDATA
#define CLAMPJUSTDATA

#include "mt310s2justdata.h"
#include "senserange.h"
#include "permissionfunctions.h"

// a clamp range consists of 2 stages . 1st the clamp itself and 2nd a voltage input range
// assigned to the clamp. so we need an interface for the clamp adjustment data (same as
// a normal senserange) but the adjustment data is the combination of the clamp's and the
// the voltage input range's adjustment data

class cSCPI;

class cClampJustData: public JustRangeTripletOffsetGainPhaseMt310s2
{
    Q_OBJECT

public:
    cClampJustData(cSCPI* scpiinterface,
                   cSenseRange* cascadedRange,
                   double cvRatio,
                   PermissionStructAdj permission = PermissionStructAdj());

protected:
    virtual double getGainCorrection(double par) override;
    virtual double getJustGainCorrection(double par) override;
    virtual double getPhaseCorrection(double par) override;
    virtual double getJustPhaseCorrection(double par) override;
    virtual double getOffsetCorrection(double par) override;
    virtual double getJustOffsetCorrection(double par) override;

private:
    cSenseRange* m_pFirstStageRange; //
    double m_cvRatio;
};

#endif // CLAMPJUSTDATA

