#include "adjdatarangestream.h"

AdjDataRange AdjDataRangeStream::fromStream(QDataStream &qds)
{
    AdjDataRange rangeAdjData;
    AdjDataItem::fromStream(rangeAdjData.m_gainAdjData, qds);
    AdjDataItem::fromStream(rangeAdjData.m_phasAdjData, qds);
    AdjDataItem::fromStream(rangeAdjData.m_offsAdjData, qds);
    return rangeAdjData;
}

void AdjDataRangeStream::toStream(AdjDataRange adjGroup, QDataStream &qds)
{
    AdjDataItem::toStream(adjGroup.m_gainAdjData, qds);
    AdjDataItem::toStream(adjGroup.m_phasAdjData, qds);
    AdjDataItem::toStream(adjGroup.m_offsAdjData, qds);
}
