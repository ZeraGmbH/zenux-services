#include "mt310s2senserange.h"
#include "rangeadjinterface.h"
#include <math.h>

static constexpr int rejectionScpiQueryDigitsMt310s2 = 8;
static constexpr int adcRejectionCom5003 = (1<<23) -1;

Mt310s2SenseRange::Mt310s2SenseRange(cSCPI *scpiinterface,
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
        adcRejectionCom5003,
        rselcode,
        mmask,
        justdata,
        rejectionScpiQueryDigitsMt310s2)
{
}

void Mt310s2SenseRange::setMModeToUpdateAvailability(int mode)
{
    m_bAvail = ((getMMask() & mode) > 0);
}
