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

void AdjustmentDataSerializer::setStatus(quint8 status)
{
    m_adjStatus = status;
}

void AdjustmentDataSerializer::setAdjNodes(QList<AdjustmentNode> adjNodes)
{
    m_adjNodes = adjNodes;
}

void AdjustmentDataSerializer::setAdjCoeff(QList<double> adjCoefficients)
{
    m_adjCoefficients = adjCoefficients;
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

void AdjustmentRangeSerializer::setInitialDataSerializer()
{
    QList<double> initialAdjCoeff = QList<double>() << 1.000000000000 << 0.000000000000 << 0.000000000000 << 0.000000000000;
    AdjustmentNode initialAdjNode(1.000000, 0.000000);
    QList<AdjustmentNode> adjNodesList = QList<AdjustmentNode>() << initialAdjNode << initialAdjNode << initialAdjNode << initialAdjNode;

    m_gainSerializer.setStatus(0);
    m_gainSerializer.setAdjCoeff(initialAdjCoeff);
    m_gainSerializer.setAdjNodes(adjNodesList);

    m_phaseSerializer.setStatus(0);
    m_phaseSerializer.setAdjCoeff(initialAdjCoeff);
    m_phaseSerializer.setAdjNodes(adjNodesList);

    m_offsetSerializer.setStatus(0);
    m_offsetSerializer.setAdjCoeff(initialAdjCoeff);
    m_offsetSerializer.setAdjNodes(adjNodesList);
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

