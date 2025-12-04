#include "adjdatanode.h"

AdjDataNode::AdjDataNode() :
    m_fCorrection(0.0),
    m_fArgument(0.0)
{
}

AdjDataNode::AdjDataNode(double corr, double arg) :
    m_fCorrection(corr),
    m_fArgument(arg)
{
}

void AdjDataNode::toStream(QDataStream& stream) const
{
    stream << m_fCorrection << m_fArgument;
}

void AdjDataNode::fromStream(QDataStream& stream)
{
    stream >> m_fCorrection >> m_fArgument;
}

QString AdjDataNode::toString(int digits) const
{
    return QString("%1;%2;").
        arg(m_fCorrection, 0, 'f', digits).
        arg(m_fArgument, 0, 'f',digits);
}

void AdjDataNode::fromString(const QString& str)
{
    m_fCorrection = str.section(';', 0, 0).toDouble();
    m_fArgument = str.section(';', 1, 1).toDouble();
}

double AdjDataNode::getCorrection() const
{
    return m_fCorrection;
}

double AdjDataNode::getArgument() const
{
    return m_fArgument;
}
