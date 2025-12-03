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

void AdjDataRange::fromStream(QDataStream &qds)
{
    m_gainAdjData.fromStream(qds);
    m_phasAdjData.fromStream(qds);
    m_offsAdjData.fromStream(qds);
}

void AdjDataRange::toStream(QDataStream &qds) const
{
    m_gainAdjData.toStream(qds);
    m_phasAdjData.toStream(qds);
    m_offsAdjData.toStream(qds);
}
