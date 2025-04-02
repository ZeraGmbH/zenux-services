#include "clampsenserange.h"
#include "adjrangescpi.h"

static constexpr int rejectionScpiQueryDigitsClamp = 8;

ClampSenseRange::ClampSenseRange(std::shared_ptr<cSCPI> scpiinterface,
                                 const QString &name,
                                 bool avail,
                                 double rValue,
                                 double rejection,
                                 double ovrejection,
                                 quint8 rselcode,
                                 quint16 mmask,
                                 AdjRangeScpi* justdata) :
    SenseRangeCommon(
        scpiinterface,
        name,
        avail,
        rValue,
        rejection,
        ovrejection,
        rselcode,
        mmask,
        justdata,
        rejectionScpiQueryDigitsClamp)
{
}
