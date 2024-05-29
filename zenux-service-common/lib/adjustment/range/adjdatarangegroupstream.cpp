#include "adjdatarangegroupstream.h"
#include "adjdataitemstream.h"

AdjDataRangeGroup AdjDataRangeGroupStream::fromStream(QDataStream &qds)
{
    AdjDataRangeGroup rangeAdjData;
    AdjDataItemStream::fromStream(rangeAdjData.m_gainAdjData, qds);
    AdjDataItemStream::fromStream(rangeAdjData.m_phasAdjData, qds);
    AdjDataItemStream::fromStream(rangeAdjData.m_offsAdjData, qds);
    return rangeAdjData;
}

void AdjDataRangeGroupStream::toStream(AdjDataRangeGroup adjGroup, QDataStream &qds)
{
    AdjDataItemStream::toStream(adjGroup.m_gainAdjData, qds);
    AdjDataItemStream::toStream(adjGroup.m_phasAdjData, qds);
    AdjDataItemStream::toStream(adjGroup.m_offsAdjData, qds);
}
