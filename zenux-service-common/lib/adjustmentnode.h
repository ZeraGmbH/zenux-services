#ifndef ADJUTMENTNODE_H
#define ADJUTMENTNODE_H

#include <QDataStream>
#include <QString>

// an adjustmentnode (datapoint) consists of the data point's value and the data point's argument
// it can serialize and deserialize itself to a qdatastream

class AdjustmentNode { // stützspunkt kann sich serialisieren und besteht aus stützwert (correction) und argument
public:
    AdjustmentNode(double corr, double arg);
    AdjustmentNode(){};
    ~AdjustmentNode(){};
    void Serialize(QDataStream&);
    void Deserialize(QDataStream&);
    QString Serialize(int digits);
    void Deserialize(const QString&);
    AdjustmentNode& operator = (const AdjustmentNode&);
    void setCorrection(double value);
    double getCorrection();
    void setArgument(double value);
    double getArgument();

private:
    double m_fCorrection;
    double m_fArgument;
};

#endif // ADJUTMENTNODE_H
