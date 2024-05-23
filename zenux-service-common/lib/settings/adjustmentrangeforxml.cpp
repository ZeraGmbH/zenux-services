#include "adjustmentrangeforxml.h"

AdjustmentRangeForXML::AdjustmentRangeForXML(int order, int *digits, quint8 *adjStatus, QList<AdjustmentNode>* adjNodes, QList<double> *adjCoefficients) :
    m_order(order),
    m_digits(digits),
    m_adjStatus(adjStatus),
    m_adjNodes(adjNodes),
    m_adjCoefficients(adjCoefficients)
{
}

QString AdjustmentRangeForXML::statusToString()
{
    return QString("%1").arg(*m_adjStatus);
}

void AdjustmentRangeForXML::statusFromString(const QString &s)
{
    *m_adjStatus = s.toInt();
}

QString AdjustmentRangeForXML::coefficientsToString()
{
    QString s;
    for (int i = 0; i < m_order+1; i++)
        s += QString("%1;").arg((*m_adjCoefficients)[i],0,'f',12);
    return s;
}

void AdjustmentRangeForXML::coefficientsFromString(const QString &s)
{
    for (int i = 0; i < m_order+1; i++)
        (*m_adjCoefficients)[i] = s.section(';',i,i).toDouble();
}

QString AdjustmentRangeForXML::nodesToString()
{
    QString s;
    for (int i = 0; i < m_order+1; i++)
        s += (*m_adjNodes)[i].toString(*m_digits);
    return s;
}

void AdjustmentRangeForXML::nodesFromString(const QString &s)
{
    for (int i = 0; i < m_order+1; i++)
        (*m_adjNodes)[i].fromString(s.section(';',i << 1,(i << 1) + 1));
}
