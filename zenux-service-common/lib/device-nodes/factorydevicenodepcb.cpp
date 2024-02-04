#include "factorydevicenodepcb.h"
#include "singletondevicenodepcbctrl.h"
#include "singletondevicenodepcbmsg.h"

AbstractDeviceNodePcbCtrlPtr FactoryDeviceNodePcb::getPcbCtrlDeviceNode()
{
    return SingletonDeviceNodePcbCtrl::getInstancePtr();
}

AbstractDeviceNodePcbMsgPtr FactoryDeviceNodePcb::getPcbMsgDeviceNode()
{
    return SingletonDeviceNodePcbMsg::getInstancePtr();
}
