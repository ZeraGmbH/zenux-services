#include "adjustmentnode.h"
#include <QDataStream>
#include <QString>

AdjustmentNode::AdjustmentNode(double corr, double arg)
    :m_fCorrection(corr),m_fArgument(arg)
{
}

void AdjustmentNode::Serialize(QDataStream& qds)
{
    qds << m_fCorrection << m_fArgument;
}

void AdjustmentNode::Deserialize(QDataStream& qds)
{
    qds >> m_fCorrection >> m_fArgument;
}

QString AdjustmentNode::toString(int digits)
{
    QString s = QString("%1;%2;").arg(m_fCorrection,0,'f',digits)
                           .arg(m_fArgument,0,'f',digits);
    return s;
}

void AdjustmentNode::fromString(const QString& s)
{
    m_fCorrection = s.section(';', 0, 0).toDouble();
    m_fArgument = s.section(';', 1, 1).toDouble();
}

double AdjustmentNode::getCorrection()
{
    return m_fCorrection;
}

double AdjustmentNode::getArgument()
{
    return m_fArgument;
}
