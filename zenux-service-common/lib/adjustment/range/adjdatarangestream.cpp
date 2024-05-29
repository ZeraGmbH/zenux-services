#include "adjdatarangestream.h"
#include "adjdataitemstream.h"

AdjDataRange AdjDataRangeStream::fromStream(QDataStream &qds)
{
    AdjDataRange rangeAdjData;
    AdjDataItemStream::fromStream(rangeAdjData.m_gainAdjData, qds);
    AdjDataItemStream::fromStream(rangeAdjData.m_phasAdjData, qds);
    AdjDataItemStream::fromStream(rangeAdjData.m_offsAdjData, qds);
    return rangeAdjData;
}

void AdjDataRangeStream::toStream(AdjDataRange adjGroup, QDataStream &qds)
{
    AdjDataItemStream::toStream(adjGroup.m_gainAdjData, qds);
    AdjDataItemStream::toStream(adjGroup.m_phasAdjData, qds);
    AdjDataItemStream::toStream(adjGroup.m_offsAdjData, qds);
}
