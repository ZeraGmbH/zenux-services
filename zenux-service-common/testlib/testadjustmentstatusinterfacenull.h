#ifndef TESTADJUSTMENTSTATUSINTERFACENULL_H
#define TESTADJUSTMENTSTATUSINTERFACENULL_H

#include <adjustmentstatusinterface.h>

class TestAdjustmentStatusInterfaceNull : public AdjustmentStatusInterface
{
public:
    quint8 getAdjustmentStatus() override;
};

#endif // TESTADJUSTMENTSTATUSINTERFACENULL_H
