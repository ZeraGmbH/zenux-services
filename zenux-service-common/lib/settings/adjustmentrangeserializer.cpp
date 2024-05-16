#include "adjustmentrangeserializer.h"
#include "rangeadjinterface.h"

AdjustmentDataSerializer::AdjustmentDataSerializer()
{
}

AdjustmentDataSerializer::AdjustmentDataSerializer(int order) :
    m_order(order)
{
}

void AdjustmentDataSerializer::Deserialize(QDataStream &qds)
{
    qds >> m_adjStatus;
    m_adjCoefficients.clear();
    for (int i = 0; i < m_order+1; i++) {
        double coeff;
        qds >> coeff;
        m_adjCoefficients.append(coeff);
    }
    m_adjNodes.clear();
    for (int i = 0; i < m_order+1; i++) {
        AdjustmentNode node;
        node.Deserialize(qds);
        m_adjNodes.append(node);
    }
}

AdjustmentRangeSerializer::AdjustmentRangeSerializer() :
    m_gainSerializer(GainCorrOrder),
    m_phaseSerializer(PhaseCorrOrder),
    m_offsetSerializer(OffsetCorrOrder)
{
}

void AdjustmentRangeSerializer::Deserialize(QDataStream &qds)
{
    m_gainSerializer.Deserialize(qds);
    m_phaseSerializer.Deserialize(qds);
    m_offsetSerializer.Deserialize(qds);
}

AdjustmentDataSerializer AdjustmentRangeSerializer::getGainSerializer()
{
    return m_gainSerializer;
}

AdjustmentDataSerializer AdjustmentRangeSerializer::getPhaseSerializer()
{
    return m_phaseSerializer;
}

AdjustmentDataSerializer AdjustmentRangeSerializer::getOffsetSerializer()
{
    return m_offsetSerializer;
}

