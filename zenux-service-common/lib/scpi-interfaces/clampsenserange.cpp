#include "clampsenserange.h"
#include "rangeadjinterface.h"

static constexpr int rejectionScpiQueryDigitsClamp = 8;
static constexpr int adcRejectionClamp = (1<<23) -1;

ClampSenseRange::ClampSenseRange(cSCPI *scpiinterface,
                                 QString name,
                                 bool avail,
                                 double rValue,
                                 double rejection,
                                 double ovrejection,
                                 quint8 rselcode,
                                 quint16 mmask,
                                 RangeAdjInterface* justdata) :
    SenseRangeCommon(
        scpiinterface,
        name,
        avail,
        rValue,
        rejection,
        ovrejection,
        adcRejectionClamp,
        rselcode,
        mmask,
        justdata,
        rejectionScpiQueryDigitsClamp)
{
}
