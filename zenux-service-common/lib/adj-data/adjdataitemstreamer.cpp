#include "adjdataitemstreamer.h"

AdjDataItemStreamer::AdjDataItemStreamer(int order) :
    m_order(order)
{
}

AdjDataItem AdjDataItemStreamer::Deserialize(QDataStream &qds)
{
    AdjDataItem adjData;
    adjData.m_order = m_order;
    qds >> adjData.m_adjStatus;
    for (int i = 0; i < m_order+1; i++) {
        double coeff;
        qds >> coeff;
        adjData.m_adjCoefficients.append(coeff);
    }
    for (int i = 0; i < m_order+1; i++) {
        double correction, argument;
        qds >> correction >> argument;
        AdjustmentNode node(correction, argument);
        adjData.m_adjNodes.append(node);
    }
    return adjData;
}
