#include "mt310s2senserange.h"
#include "adjrangescpi.h"
#include <math.h>

static constexpr int rejectionScpiQueryDigitsMt310s2 = 8;
static constexpr int adcRejectionMt310s2 = (1<<23) -1;

Mt310s2SenseRange::Mt310s2SenseRange(std::shared_ptr<cSCPI> scpiinterface,
                                     QString name,
                                     bool avail,
                                     double rValue,
                                     double rejection,
                                     double ovrejection,
                                     quint8 rselcode,
                                     quint16 mmask) :
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
        new AdjRangeScpi(scpiinterface, AdjustScpiValueFormatterFactory::createMt310s2AdjFormatter()),
        rejectionScpiQueryDigitsMt310s2)
{
}
