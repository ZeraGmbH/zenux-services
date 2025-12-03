#include "adjdataitem.h"
#include <gaussmatrix.h>
#include <gaussnode.h>
#include <math.h>

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

void AdjDataItem::initData(double init)
{
    setNode(0, AdjDataNode(init, 0.0)); // setting the 1st node and all following
    calcCoefficientsFromNodes();
    m_adjStatus = 0;
}

bool AdjDataItem::setNode(int index, AdjDataNode jn)
{
    if (index < getOrder()+1) {
        for (int i = index; i < getOrder()+1; i++)
            m_adjNodes[i] = jn;
        return true;
    }
    return false;
}

AdjDataNode *AdjDataItem::getNode(int index)
{
    return &m_adjNodes[index];
}

bool AdjDataItem::setCoefficient(int index, double value)
{
    if (index < getOrder()+1) {
        m_adjCoefficients[index] = value;
        if (index < getOrder())
            for (int i = index+1; i < getOrder()+1; i++)
                m_adjCoefficients[i] = 0.0;
        return true;
    }
    return false;
}

double AdjDataItem::getCoefficient(int index)
{
    return m_adjCoefficients[index];
}

bool AdjDataItem::calcCoefficientsFromNodes()
{
    const double epsilon = 1e-7;
    int i;
    int realOrd = 0;
    if (getOrder() > 0) { // only done if noticed order > 0
        for (i = 0;i < getOrder(); i++) {
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
    for (i = i; i < getOrder()+1; i++)
        setCoefficient(i, 0.0);
    delete Matrix;
    return true;
}

double AdjDataItem::getCorrection(double arg)
{
    double Arg = 1.0;
    double Corr = 0.0;
    for (int i = 0; i < getOrder()+1; i++) { // correction function has nth order
        Corr += m_adjCoefficients[i] * Arg;
        Arg *= arg;
    }
    return Corr;
}

void AdjDataItem::fromStream(AdjDataItem &item, QDataStream &qds)
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

void AdjDataItem::toStream(AdjDataItem item, QDataStream &qds)
{
    qds << item.m_adjStatus;
    for (int i = 0; i < item.m_adjCoefficients.size(); i++)
        qds << item.m_adjCoefficients[i];
    for (int i = 0; i < item.m_adjNodes.size(); i++)
        qds << item.m_adjNodes[i].getCorrection() << item.m_adjNodes[i].getArgument();
}
