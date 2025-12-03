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

AdjDataRange AdjDataRange::fromStream(QDataStream &qds)
{
    AdjDataRange rangeAdjData;
    AdjDataItem::fromStream(rangeAdjData.m_gainAdjData, qds);
    AdjDataItem::fromStream(rangeAdjData.m_phasAdjData, qds);
    AdjDataItem::fromStream(rangeAdjData.m_offsAdjData, qds);
    return rangeAdjData;
}

void AdjDataRange::toStream(QDataStream &qds) const
{
    m_gainAdjData.toStream(qds);
    m_phasAdjData.toStream(qds);
    m_offsAdjData.toStream(qds);
}
