#ifndef ADJDATARANGEGROUP_H
#define ADJDATARANGEGROUP_H

#include "adjdataitem.h"
#include "adjflags.h"

class AdjDataRangeGroup
{
public:
    AdjDataRangeGroup() :
        m_gainAdjData(GainCorrOrder),
        m_phasAdjData(PhaseCorrOrder),
        m_offsAdjData(OffsetCorrOrder)
    {}
    AdjDataItem m_gainAdjData;
    AdjDataItem m_phasAdjData;
    AdjDataItem m_offsAdjData;
};

#endif // ADJDATARANGEGROUP_H
