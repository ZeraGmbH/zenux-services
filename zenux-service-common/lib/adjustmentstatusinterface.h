#ifndef ADJUSTMENTSTATUSINTERFACE_H
#define ADJUSTMENTSTATUSINTERFACE_H

#include <QtGlobal>

class AdjustmentStatusInterface
{
public:
    virtual ~AdjustmentStatusInterface() = default;
    virtual quint8 getAdjustmentStatus() = 0;
};

#endif // ADJUSTMENTSTATUSINTERFACE_H
