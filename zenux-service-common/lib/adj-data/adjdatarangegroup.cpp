#include "adjdatarangegroup.h"
#include "adjflags.h"

AdjDataRangeGroup::AdjDataRangeGroup() :
    m_gainAdjData(GainCorrOrder),
    m_phasAdjData(PhaseCorrOrder),
    m_offsAdjData(OffsetCorrOrder)
{
    initGroup();
}

void AdjDataRangeGroup::initGroup()
{
    m_gainAdjData.initData(1.0);
    m_phasAdjData.initData(0.0);
    m_offsAdjData.initData(0.0);
}

