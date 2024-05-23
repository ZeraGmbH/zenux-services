#ifndef ADJUSTMENTRANGEDATA_H
#define ADJUSTMENTRANGEDATA_H

#include "adjustmentnode.h"
#include <QList>

class AdjustmentTypeData
{
public:
    int m_order;
    quint8 m_adjStatus;
    QList<AdjustmentNode> m_adjNodes;
    QList<double> m_adjCoefficients;
};

class AdjustmentRangeData
{
public:
    AdjustmentTypeData m_gainAdjData;
    AdjustmentTypeData m_phasAdjData;
    AdjustmentTypeData m_offsAdjData;
};

#endif // ADJUSTMENTRANGEDATA_H
