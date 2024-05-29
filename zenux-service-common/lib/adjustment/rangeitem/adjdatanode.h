#ifndef ADJUTMENTNODE_H
#define ADJUTMENTNODE_H

#include <QDataStream>
#include <QString>

// an adjustmentnode (datapoint) consists of the data point's value and the data point's argument
// it can serialize and deserialize itself to a qdatastream

class AdjDataNode
{
public:
    AdjDataNode(double corr, double arg);
    AdjDataNode(){};
    void toStream(QDataStream&);
    void fromStream(QDataStream&);

    QString toString(int digits);
    void fromString(const QString&);

    double getCorrection();
    double getArgument();

private:
    double m_fCorrection;
    double m_fArgument;
};

#endif // ADJUTMENTNODE_H
