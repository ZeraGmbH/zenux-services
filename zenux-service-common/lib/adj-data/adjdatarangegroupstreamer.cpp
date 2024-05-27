#include "adjdatarangegroupstreamer.h"
#include "adjdataitemstreamer.h"

std::shared_ptr<AdjDataRangeGroup> AdjDataRangeGroupStreamer::Deserialize(QDataStream &qds)
{
    std::shared_ptr<AdjDataRangeGroup> rangeAdjData = std::make_shared<AdjDataRangeGroup>();
    AdjDataItemStreamer::Deserialize(rangeAdjData->m_gainAdjData, qds);
    AdjDataItemStreamer::Deserialize(rangeAdjData->m_phasAdjData, qds);
    AdjDataItemStreamer::Deserialize(rangeAdjData->m_offsAdjData, qds);

    return rangeAdjData;
}
