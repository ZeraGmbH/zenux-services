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

    static AdjDataRange fromStream(QDataStream &qds);
    static void toStream(AdjDataRange adjGroup, QDataStream &qds);

    AdjDataItem m_gainAdjData;
    AdjDataItem m_phasAdjData;
    AdjDataItem m_offsAdjData;
};

#endif // ADJDATARANGE_H
