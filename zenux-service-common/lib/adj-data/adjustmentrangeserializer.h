#ifndef ADJUSTMENTRANGESERIALIZER_H
#define ADJUSTMENTRANGESERIALIZER_H

#include "adjustmentnode.h"
#include "adjustmentdatarangegroup.h"
#include <QDataStream>
#include <memory>

class AdjustmentDataSerializer
{
public:
    AdjustmentDataSerializer(int order);
    AdjustmentDataItem Deserialize(QDataStream& qds);
private:
    int m_order;
};

class AdjustmentRangeSerializer
{
public:
    static std::shared_ptr<AdjustmentDataRangeGroup> Deserialize(QDataStream& qds);
};

#endif // ADJUSTMENTRANGESERIALIZER_H
