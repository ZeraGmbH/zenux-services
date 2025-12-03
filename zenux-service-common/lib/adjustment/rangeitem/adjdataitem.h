#ifndef ADJDATAITEM_H
#define ADJDATAITEM_H

#include "adjdatanode.h"
#include <QVector>

// An AdjDataItem object has a max. possible order
// the order must not necessarily be used
// setting only the first node results in a effectively 0 order
// a new generated object is also initialized like that

class AdjDataItem
{
public:
    explicit AdjDataItem(int order);
    int getOrder() const;

    void initData(double init);

    quint8 getAdjStatus() const;
    void setAdjStatus(quint8 adjStatus);

    bool setNode(int index, const AdjDataNode &jn); // !!! setting node sequence is relevant !!!
    const AdjDataNode &getNode(int index) const;

    bool setCoefficient(int index, double value); // !!! setting coefficient also is sequence relevant !!!
    double getCoefficient(int index) const;

    void computeCoefficientsFromNodes();
    double getCorrection(double arg) const; // calculates correction value c= ax^order +bx^order-1 ...

    void fromStream(QDataStream &qds);
    void toStream(QDataStream &qds) const;

    void nodesFromString(const QString &s);
    QString nodesToString(int digits) const;

    void coefficientsFromString(const QString &s);
    QString coefficientsToString() const;

private:
    int m_order;
    quint8 m_adjStatus = 0;
    QVector<AdjDataNode> m_adjNodes;
    QVector<double> m_adjCoefficients;
};


#endif // ADJDATAITEM_H
