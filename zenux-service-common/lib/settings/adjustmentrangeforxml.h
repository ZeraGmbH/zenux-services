#ifndef ADJUSTMENTRANGEFORXML_H
#define ADJUSTMENTRANGEFORXML_H

#include "adjustmentnode.h"
#include "QObject"
#include<memory>

class AdjustmentRangeForXML
{
public:
    AdjustmentRangeForXML(int order, int* digits, quint8* adjStatus, QList<AdjustmentNode> *adjNodes, QList<double>* adjCoefficients);

    QString statusToString();
    void statusFromString(const QString& s);
    QString coefficientsToString();
    void coefficientsFromString(const QString& s);
    QString nodesToString();
    void nodesFromString(const QString& s );

private:
    int m_order;
    int *m_digits;
    quint8 *m_adjStatus;
    QList<AdjustmentNode>* m_adjNodes;
    QList<double>* m_adjCoefficients;
};

#endif // ADJUSTMENTRANGEFORXML_H
