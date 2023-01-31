#ifndef ADJUSTMENTSTATUSNULL_H
#define ADJUSTMENTSTATUSNULL_H

#include <adjustmentstatusinterface.h>

class AdjustmentStatusNull : public AdjustmentStatusInterface
{
public:
    quint8 getAdjustmentStatus() override;
};

#endif // ADJUSTMENTSTATUSNULL_H
