#include "adjdataitem.h"

AdjDataItem::AdjDataItem(int order) :
    m_adjNodes(order+1),
    m_adjCoefficients(order+1),
    m_order(order)
{
    Q_ASSERT(order+1 > 0);
}

int AdjDataItem::getOrder()
{
    return m_order;
}
