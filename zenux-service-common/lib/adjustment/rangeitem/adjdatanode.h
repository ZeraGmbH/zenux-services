#ifndef ADJUTMENTNODE_H
#define ADJUTMENTNODE_H

#include <QDataStream>
#include <QString>

// an adjustmentnode (datapoint) consists of the data point's value and the data point's argument
// it can serialize and deserialize itself to a qdatastream

class AdjDataNode
{
public:
    AdjDataNode();
    AdjDataNode(double corr, double arg);

    void toStream(QDataStream& stream) const;
    void fromStream(QDataStream& stream);

    QString toString(int digits) const;
    void fromString(const QString& str);

    double getCorrection() const;
    double getArgument() const;

private:
    double m_fCorrection;
    double m_fArgument;
};

#endif // ADJUTMENTNODE_H
