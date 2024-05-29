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
    AdjDataItem(int order);
    int getOrder();

    void initData(double init);
    
    bool setNode(int index, AdjDataNode jn); // !!! setting node sequence is relevant !!!
    AdjDataNode* getNode(int index);

    bool setCoefficient(int index, double value); // !!! setting coefficient also is sequence relevant !!!
    double getCoefficient(int index);

    bool calcCoefficientsFromNodes();
    double getCorrection(double arg); // calculates correction value c= ax^order +bx^order-1 ...

    quint8 m_adjStatus = 0;
    QVector<AdjDataNode> m_adjNodes;
    QVector<double> m_adjCoefficients;
private:
    int m_order;
};


#endif // ADJDATAITEM_H
