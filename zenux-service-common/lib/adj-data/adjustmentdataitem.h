#ifndef ADJUSTMENTDATAITEM_H
#define ADJUSTMENTDATAITEM_H

#include "adjustmentnode.h"
#include <QList>

class AdjustmentDataItem
{
public:
    int m_order;
    quint8 m_adjStatus;
    QList<AdjustmentNode> m_adjNodes;
    QList<double> m_adjCoefficients;
};

#endif // ADJUSTMENTDATAITEM_H
