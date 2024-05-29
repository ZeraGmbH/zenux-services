#ifndef ADJDATARANGEGROUP_H
#define ADJDATARANGEGROUP_H

#include "adjdataitem.h"

class AdjDataRangeGroup
{
public:
    AdjDataRangeGroup();
    AdjDataItem m_gainAdjData;
    AdjDataItem m_phasAdjData;
    AdjDataItem m_offsAdjData;
};

#endif // ADJDATARANGEGROUP_H
