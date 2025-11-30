#ifndef ADJDATARANGE_H
#define ADJDATARANGE_H

#include "adjdataitem.h"

class AdjDataRange
{
public:
    AdjDataRange();
    void initGain();
    void initPhase();
    void initOffset();
    AdjDataItem m_gainAdjData;
    AdjDataItem m_phasAdjData;
    AdjDataItem m_offsAdjData;
};

#endif // ADJDATARANGE_H
