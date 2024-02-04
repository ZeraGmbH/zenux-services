#include "mockfactorydevicenodepcb.h"
#include "mocksingletondevicenodepcbctrl.h"
#include "mocksingletondevicenodepcbmsg.h"

AbstractDeviceNodePcbCtrlPtr MockFactoryDeviceNodePcb::getPcbCtrlDeviceNode()
{
    return MockSingletonDeviceNodePcbCtrl::getInstancePtr();
}

AbstractDeviceNodePcbMsgPtr MockFactoryDeviceNodePcb::getPcbMsgDeviceNode()
{
    return MockSingletonDeviceNodePcbMsg::getInstancePtr();
}
