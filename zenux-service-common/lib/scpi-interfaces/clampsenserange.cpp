#include "clampsenserange.h"
#include "adjrangescpi.h"

static constexpr int rejectionScpiQueryDigitsClamp = 8;

ClampSenseRange::ClampSenseRange(const std::shared_ptr<cSCPI> &scpiInterface,
                                 const QString &name,
                                 bool avail,
                                 double rValue,
                                 double rejection,
                                 double ovrejection,
                                 quint8 rselcode,
                                 quint16 mmask,
                                 AdjRangeScpi* justdata) :
    SenseRangeCommon(
        scpiInterface,
        avail,
        {
            name,
            rValue,
            rejection,
            ovrejection,
            rselcode,
            mmask,
            rejectionScpiQueryDigitsClamp
        },
        justdata
)
{
}
