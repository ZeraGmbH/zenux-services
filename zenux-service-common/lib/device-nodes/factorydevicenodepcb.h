#ifndef FACTORYDEVICENODEPCB_H
#define FACTORYDEVICENODEPCB_H

#include "abstractfactorydevicenodepcb.h"

class FactoryDeviceNodePcb : public AbstractFactoryDeviceNodePcb
{
public:
    AbstractDeviceNodePcbCtrlPtr getPcbCtrlDeviceNode() override;
    AbstractDeviceNodePcbMsgPtr getPcbMsgDeviceNode() override;
};

#endif // FACTORYDEVICENODEPCB_H
