#include "adjustmentrangeserializer.h"
#include "rangeadjinterface.h"

AdjustmentDataSerializer::AdjustmentDataSerializer(int order) :
    m_order(order)
{
}

AdjustmentTypeData AdjustmentDataSerializer::Deserialize(QDataStream &qds)
{
    AdjustmentTypeData adjData;
    adjData.m_order = m_order;
    qds >> adjData.m_adjStatus;
    for (int i = 0; i < m_order+1; i++) {
        double coeff;
        qds >> coeff;
        adjData.m_adjCoefficients.append(coeff);
    }
    for (int i = 0; i < m_order+1; i++) {
        AdjustmentNode node;
        node.Deserialize(qds);
        adjData.m_adjNodes.append(node);
    }
    return adjData;
}

std::shared_ptr<AdjustmentRangeData> AdjustmentRangeSerializer::Deserialize(QDataStream &qds)
{
    AdjustmentDataSerializer gainSerializer(GainCorrOrder);
    AdjustmentDataSerializer phaseSerializer(PhaseCorrOrder);
    AdjustmentDataSerializer offsetSerializer(OffsetCorrOrder);

    std::shared_ptr<AdjustmentRangeData> rangeAdjData = std::make_shared<AdjustmentRangeData>();
    rangeAdjData->m_gainAdjData = gainSerializer.Deserialize(qds);
    rangeAdjData->m_phasAdjData = phaseSerializer.Deserialize(qds);
    rangeAdjData->m_offsAdjData = offsetSerializer.Deserialize(qds);

    return rangeAdjData;
}
