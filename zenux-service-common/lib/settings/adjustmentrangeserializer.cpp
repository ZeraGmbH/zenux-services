#include "adjustmentrangeserializer.h"
#include "rangeadjinterface.h"
#include <gaussmatrix.h>
#include <gaussnode.h>
#include <math.h>

AdjustmentDataSerializer::AdjustmentDataSerializer(int order) :
    m_order(order),
    m_adjustmentRangeXml(m_order, &m_digits, &m_adjStatus, &m_adjNodes, &m_adjCoefficients)
{
}

void AdjustmentDataSerializer::Deserialize(QDataStream &qds)
{
    qds >> m_adjStatus;
    m_digits = 20;
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

void AdjustmentDataSerializer::Serialize(QDataStream &qds)
{
    qds << m_adjStatus;
    int i;
    for (i = 0; i < m_order+1; i++)
        qds << m_adjCoefficients[i];
    for (i = 0; i < m_order+1; i++)
        m_adjNodes[i].Serialize(qds);
}

bool AdjustmentDataSerializer::calcCoefficientsFromNodes()
{
    const double epsilon = 1e-7;
    int i;
    int realOrd = 0;
    if (m_order > 0) { // only done if noticed order > 0
        for (i = 0;i < m_order; i++) {
            if (fabs(m_adjNodes[i].getArgument() - m_adjNodes[i+1].getArgument()) < epsilon)
                break;
            realOrd++;
        }
    }
    // fill the matrix
    cGaussMatrix *Matrix;
    Matrix = new cGaussMatrix(realOrd+1);
    cGaussNode gn;
    for (i = 0; i < realOrd+1; i++) {
        gn.m_fNode = m_adjNodes[i].getCorrection();
        gn.m_fArg = m_adjNodes[i].getArgument();
        Matrix->setMatrix(i, gn);
    }
    // matrix computation
    Matrix->cmpKoeff();
    // read out coefficients
    for (i = 0; i < realOrd+1; i++)
        setCoefficient(i, Matrix->getKoeff(realOrd-i));
    // not calculated coefficient are set to 0
    for (i = i; i < m_order+1; i++)
    setCoefficient(i, 0.0);
    delete Matrix;
    return true;

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

void AdjustmentDataSerializer::setDigits(int digits)
{
    m_digits = digits;
}

double AdjustmentDataSerializer::getCorrection(double arg)
{
    double Arg = 1.0;
    double Corr = 0.0;
    for (int i = 0; i < m_order+1; i++) {
        Corr += m_adjCoefficients[i] * Arg;
        Arg *= arg;
    }
    return Corr;
}

quint8 AdjustmentDataSerializer::getStatus()
{
    return m_adjStatus;
}

AdjustmentRangeForXML AdjustmentDataSerializer::getAdjRangeForXML()
{
    return m_adjustmentRangeXml;
}

void AdjustmentDataSerializer::initJustData(double init)
{
    setNode(0 , AdjustmentNode(init,0.0)); // setting the 1st node and all following
    calcCoefficientsFromNodes();
    m_adjStatus = 0;
}

bool AdjustmentDataSerializer::setCoefficient(int index, double value)
{
    if (index < m_order+1) {
        m_adjCoefficients[index] = value;
        if (index < m_order)
            for (int i = index+1; i < m_order+1; i++)
                m_adjCoefficients[i] = 0.0;
        return true;
    }
    return false;
}

bool AdjustmentDataSerializer::setNode(int index, AdjustmentNode jn)
{
    if (index < m_order+1) {
        for (int i = index; i < m_order+1; i++)
            m_adjNodes[i] = jn;
        return true;
    }
    return false;
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

