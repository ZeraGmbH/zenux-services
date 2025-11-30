#include "adjdatarange.h"
#include "adjflags.h"

AdjDataRange::AdjDataRange() :
    m_gainAdjData(GainCorrOrder),
    m_phasAdjData(PhaseCorrOrder),
    m_offsAdjData(OffsetCorrOrder)
{
    initGain();
    initPhase();
    initOffset();
}

void AdjDataRange::initGain()
{
    m_gainAdjData.initData(1.0);
}

void AdjDataRange::initPhase()
{
    m_phasAdjData.initData(0.0);
}

void AdjDataRange::initOffset()
{
    m_offsAdjData.initData(0.0);
}
