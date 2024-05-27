#ifndef ADJDATAITEM_H
#define ADJDATAITEM_H

#include "adjustmentnode.h"
#include <QList>

class AdjDataItem
{
public:
    int m_order;
    quint8 m_adjStatus;
    QList<AdjustmentNode> m_adjNodes;
    QList<double> m_adjCoefficients;
};

#endif // ADJDATAITEM_H
