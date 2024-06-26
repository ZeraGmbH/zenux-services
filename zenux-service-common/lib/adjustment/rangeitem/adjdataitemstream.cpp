#include "adjdataitemstream.h"

void AdjDataItemStream::fromStream(AdjDataItem &item, QDataStream &qds)
{
    qds >> item.m_adjStatus;
    for (int i = 0; i < item.getOrder()+1; i++) {
        double coeff;
        qds >> coeff;
        item.m_adjCoefficients[i] = coeff;
    }
    for (int i = 0; i < item.getOrder()+1; i++) {
        double correction, argument;
        qds >> correction >> argument;
        AdjDataNode node(correction, argument);
        item.m_adjNodes[i] = node;
    }
}

void AdjDataItemStream::toStream(AdjDataItem item, QDataStream &qds)
{
    qds << item.m_adjStatus;
    for (int i = 0; i < item.m_adjCoefficients.size(); i++)
        qds << item.m_adjCoefficients[i];
    for (int i = 0; i < item.m_adjNodes.size(); i++)
        qds << item.m_adjNodes[i].getCorrection() << item.m_adjNodes[i].getArgument();
}
