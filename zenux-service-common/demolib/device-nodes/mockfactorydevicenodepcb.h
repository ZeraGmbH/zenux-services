#ifndef MOCKFACTORYDEVICENODEPCB_H
#define MOCKFACTORYDEVICENODEPCB_H

#include <abstractfactorydevicenodepcb.h>

class MockFactoryDeviceNodePcb : public AbstractFactoryDeviceNodePcb
{
public:
    AbstractDeviceNodePcbCtrlPtr getPcbCtrlDeviceNode() override;
    AbstractDeviceNodePcbMsgPtr getPcbMsgDeviceNode() override;
};

#endif // MOCKFACTORYDEVICENODEPCB_H
