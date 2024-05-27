#ifndef TESTADJUSTMENTSTATUSINTERFACENULL_H
#define TESTADJUSTMENTSTATUSINTERFACENULL_H

#include <abstractadjstatus.h>

class TestAdjustmentStatusInterfaceNull : public AbstractAdjStatus
{
public:
    quint8 getAdjustmentStatus() override;
};

#endif // TESTADJUSTMENTSTATUSINTERFACENULL_H
