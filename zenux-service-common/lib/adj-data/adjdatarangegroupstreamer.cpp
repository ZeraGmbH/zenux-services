#include "adjdatarangegroupstreamer.h"
#include "adjdataitemstreamer.h"

AdjDataRangeGroup AdjDataRangeGroupStreamer::Deserialize(QDataStream &qds)
{
    AdjDataRangeGroup rangeAdjData;
    AdjDataItemStreamer::Deserialize(rangeAdjData.m_gainAdjData, qds);
    AdjDataItemStreamer::Deserialize(rangeAdjData.m_phasAdjData, qds);
    AdjDataItemStreamer::Deserialize(rangeAdjData.m_offsAdjData, qds);
    return rangeAdjData;
}

void AdjDataRangeGroupStreamer::Serialize(AdjDataRangeGroup adjGroup, QDataStream &qds)
{
    AdjDataItemStreamer::Serialize(adjGroup.m_gainAdjData, qds);
    AdjDataItemStreamer::Serialize(adjGroup.m_phasAdjData, qds);
    AdjDataItemStreamer::Serialize(adjGroup.m_offsAdjData, qds);
}
