#include "com5003senserange.h"

static constexpr int rejectionScpiQueryDigitsCom5003 = 6;
static constexpr int adcRejectionCom5003 = (1<<23) -1;

Com5003SenseRange::Com5003SenseRange(std::shared_ptr<cSCPI> scpiinterface,
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
        adcRejectionCom5003,
        rselcode,
        mmask,
        new AdjRangeScpi(scpiinterface, AdjustScpiValueFormatterFactory::createCom5003AdjFormatter()),
        rejectionScpiQueryDigitsCom5003)
{
}
