#include "adjdatarangegroupstreamer.h"
#include "adjdataitemstreamer.h"
#include "rangeadjinterface.h"

std::shared_ptr<AdjDataRangeGroup> AdjDataRangeGroupStreamer::Deserialize(QDataStream &qds)
{
    AdjDataItemStreamer gainSerializer(GainCorrOrder);
    AdjDataItemStreamer phaseSerializer(PhaseCorrOrder);
    AdjDataItemStreamer offsetSerializer(OffsetCorrOrder);

    std::shared_ptr<AdjDataRangeGroup> rangeAdjData = std::make_shared<AdjDataRangeGroup>();
    rangeAdjData->m_gainAdjData = gainSerializer.Deserialize(qds);
    rangeAdjData->m_phasAdjData = phaseSerializer.Deserialize(qds);
    rangeAdjData->m_offsAdjData = offsetSerializer.Deserialize(qds);

    return rangeAdjData;
}
