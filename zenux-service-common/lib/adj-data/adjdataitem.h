#ifndef ADJDATAITEM_H
#define ADJDATAITEM_H

#include "adjustmentnode.h"
#include <QVector>

class AdjDataItem
{
public:
    AdjDataItem(int order);
    int getOrder();
    quint8 m_adjStatus;
    QVector<AdjustmentNode> m_adjNodes;
    QVector<double> m_adjCoefficients;
private:
    int m_order;
};


#endif // ADJDATAITEM_H
