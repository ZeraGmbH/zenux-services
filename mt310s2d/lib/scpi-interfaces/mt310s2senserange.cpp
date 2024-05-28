#include "mt310s2senserange.h"
#include "adjrangeinterface.h"
#include <math.h>

static constexpr int rejectionScpiQueryDigitsMt310s2 = 8;
static constexpr int adcRejectionMt310s2 = (1<<23) -1;

Mt310s2SenseRange::Mt310s2SenseRange(cSCPI *scpiinterface,
                                     QString name,
                                     bool avail,
                                     double rValue,
                                     double rejection,
                                     double ovrejection,
                                     quint8 rselcode,
                                     quint16 mmask,
                                     AdjRangeInterface* justdata) :
    SenseRangeCommon(
        scpiinterface,
        name,
        avail,
        rValue,
        rejection,
        ovrejection,
        adcRejectionMt310s2,
        rselcode,
        mmask,
        justdata,
        rejectionScpiQueryDigitsMt310s2)
{
}
