#ifndef ABSTRACTFACTORYDEVICENODEPCB_H
#define ABSTRACTFACTORYDEVICENODEPCB_H

#include "abstractdevicenodepcbctrl.h"
#include "abstractdevicenodepcbmsg.h"

class AbstractFactoryDeviceNodePcb
{
public:
    virtual AbstractDeviceNodePcbCtrlPtr getPcbCtrlDeviceNode() = 0;
    virtual AbstractDeviceNodePcbMsgPtr getPcbMsgDeviceNode() = 0;
};

typedef std::shared_ptr<AbstractFactoryDeviceNodePcb> AbstractFactoryDeviceNodePcbPtr;

#endif // ABSTRACTFACTORYDEVICENODEPCB_H
