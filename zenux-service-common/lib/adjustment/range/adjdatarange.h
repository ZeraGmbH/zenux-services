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

    void fromStream(QDataStream &qds);
    void toStream(QDataStream &qds) const;

    AdjDataItem m_gainAdjData;
    AdjDataItem m_phasAdjData;
    AdjDataItem m_offsAdjData;
};

#endif // ADJDATARANGE_H
