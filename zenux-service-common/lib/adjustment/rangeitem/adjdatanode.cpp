#include "adjdatanode.h"
#include <QDataStream>
#include <QString>

AdjDataNode::AdjDataNode(double corr, double arg) :
    m_fCorrection(corr),m_fArgument(arg)
{
}

void AdjDataNode::toStream(QDataStream& qds)
{
    qds << m_fCorrection << m_fArgument;
}

void AdjDataNode::fromStream(QDataStream& qds)
{
    qds >> m_fCorrection >> m_fArgument;
}

QString AdjDataNode::toString(int digits)
{
    QString s = QString("%1;%2;").arg(m_fCorrection,0,'f',digits)
                           .arg(m_fArgument,0,'f',digits);
    return s;
}

void AdjDataNode::fromString(const QString& s)
{
    m_fCorrection = s.section(';', 0, 0).toDouble();
    m_fArgument = s.section(';', 1, 1).toDouble();
}

double AdjDataNode::getCorrection() const
{
    return m_fCorrection;
}

double AdjDataNode::getArgument() const
{
    return m_fArgument;
}
