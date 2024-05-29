#include "adjdatarangegroup.h"
#include "adjflags.h"

AdjDataRangeGroup::AdjDataRangeGroup() :
    m_gainAdjData(GainCorrOrder),
    m_phasAdjData(PhaseCorrOrder),
    m_offsAdjData(OffsetCorrOrder)
{
}

