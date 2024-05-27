#ifndef ADJUSTMENTRANGESERIALIZER_H
#define ADJUSTMENTRANGESERIALIZER_H

#include "adjustmentnode.h"
#include "adjustmentrangedata.h"
#include <QDataStream>
#include <memory>

class AdjustmentDataSerializer
{
public:
    AdjustmentDataSerializer(int order);
    AdjustmentTypeData Deserialize(QDataStream& qds);
private:
    int m_order;
};

class AdjustmentRangeSerializer
{
public:
    static std::shared_ptr<AdjustmentRangeData> Deserialize(QDataStream& qds);
};

#endif // ADJUSTMENTRANGESERIALIZER_H
