#ifndef ADJUSTMENTRANGESERIALIZER_H
#define ADJUSTMENTRANGESERIALIZER_H

#include "adjustmentnode.h"
#include "scpiconnection.h"
#include "adjustmentrangeforxml.h"
#include "adjustmentrangeforscpi.h"
#include <QDataStream>

class AdjustmentDataSerializer
{
public:
    AdjustmentDataSerializer(int order);

    void Deserialize(QDataStream& qds);
    void Serialize(QDataStream& qds);
    bool calcCoefficientsFromNodes();

    void setStatus(quint8 status);
    void setAdjNodes(QList<AdjustmentNode> adjNodes);
    void setAdjCoeff(QList<double> adjCoefficients);
    void setDigits(int digits);

    double getCorrection(double arg); // calculates correction value c= ax^order +bx^order-1 ...
    quint8 getStatus();
    AdjustmentRangeForXML getAdjRangeForXML();

    void initJustData(double init);

private:
    bool setCoefficient(int index, double value);
    bool setNode(int index, AdjustmentNode jn);

    int m_order;
    quint8 m_adjStatus;
    int m_digits;
    QList<AdjustmentNode> m_adjNodes;
    QList<double> m_adjCoefficients;

    AdjustmentRangeForXML m_adjustmentRangeXml;
};


class AdjustmentRangeSerializer
{
public:
    AdjustmentRangeSerializer();
    void Deserialize(QDataStream& qds);
    void setInitialDataSerializer();
    AdjustmentDataSerializer getGainSerializer();
    AdjustmentDataSerializer getPhaseSerializer();
    AdjustmentDataSerializer getOffsetSerializer();

private:
    AdjustmentDataSerializer m_gainSerializer;
    AdjustmentDataSerializer m_phaseSerializer;
    AdjustmentDataSerializer m_offsetSerializer;


};

#endif // ADJUSTMENTRANGESERIALIZER_H
