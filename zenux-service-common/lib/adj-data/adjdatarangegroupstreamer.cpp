#include "adjdatarangegroupstreamer.h"
#include "adjdataitemstreamer.h"

AdjDataRangeGroup AdjDataRangeGroupStreamer::fromStream(QDataStream &qds)
{
    AdjDataRangeGroup rangeAdjData;
    AdjDataItemStreamer::fromStream(rangeAdjData.m_gainAdjData, qds);
    AdjDataItemStreamer::fromStream(rangeAdjData.m_phasAdjData, qds);
    AdjDataItemStreamer::fromStream(rangeAdjData.m_offsAdjData, qds);
    return rangeAdjData;
}

void AdjDataRangeGroupStreamer::toStream(AdjDataRangeGroup adjGroup, QDataStream &qds)
{
    AdjDataItemStreamer::toStream(adjGroup.m_gainAdjData, qds);
    AdjDataItemStreamer::toStream(adjGroup.m_phasAdjData, qds);
    AdjDataItemStreamer::toStream(adjGroup.m_offsAdjData, qds);
}
