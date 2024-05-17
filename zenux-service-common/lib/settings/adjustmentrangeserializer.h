#ifndef ADJUSTMENTRANGESERIALIZER_H
#define ADJUSTMENTRANGESERIALIZER_H

#include "adjustmentnode.h"
#include <QDataStream>

class AdjustmentDataSerializer
{
public:
    AdjustmentDataSerializer();
    AdjustmentDataSerializer(int order);
    void Deserialize(QDataStream& qds);

    void setStatus(quint8 status);
    void setAdjNodes(QList<AdjustmentNode> adjNodes);
    void setAdjCoeff(QList<double> adjCoefficients);

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
    void setInitialDataSerializer();
    AdjustmentDataSerializer getGainSerializer();
    AdjustmentDataSerializer getPhaseSerializer();
    AdjustmentDataSerializer getOffsetSerializer();

private:
    AdjustmentDataSerializer m_gainSerializer;
    AdjustmentDataSerializer m_phaseSerializer;
    AdjustmentDataSerializer m_offsetSerializer;


};

#endif // ADJUSTMENTRANGESERIALIZER_H
