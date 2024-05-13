#ifndef ADJUSTMENTRANGESERIALIZER_H
#define ADJUSTMENTRANGESERIALIZER_H

#include "adjustmentnode.h"
#include <QDataStream>

class AdjustmentDataSerializer
{
public:
    AdjustmentDataSerializer(int order);
    void Deserialize(QDataStream& qds);
private:
    int m_order;
    quint8 m_adjStatus;
    QList<AdjustmentNode> m_adjNodes;
    QList<double> m_adjCoefficients;
};


class AdjustmentRangeSerializer
{
public:
    AdjustmentRangeSerializer();
    void Deserialize(QDataStream& qds);
private:
    AdjustmentDataSerializer m_gainSerializer;
    AdjustmentDataSerializer m_phaseSerializer;
    AdjustmentDataSerializer m_offsetSerializer;


};

#endif // ADJUSTMENTRANGESERIALIZER_H
