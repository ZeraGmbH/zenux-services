#include "mtxxxs2senserange.h"

static constexpr int rejectionScpiQueryDigitsMt310s2 = 8;

MtXXXs2SenseRange::MtXXXs2SenseRange(std::shared_ptr<cSCPI> scpiinterface,
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
        rselcode,
        mmask,
        new AdjRangeScpi(scpiinterface, AdjustScpiValueFormatterFactory::createMt310s2AdjFormatter()),
        rejectionScpiQueryDigitsMt310s2)
{
}
