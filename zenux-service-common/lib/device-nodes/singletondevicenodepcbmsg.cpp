#include "singletondevicenodepcbmsg.h"
#include "devicenodepcbmsg.h"

AbstractDeviceNodePcbMsgPtr SingletonDeviceNodePcbMsg::m_instance;

AbstractDeviceNodePcbMsgPtr SingletonDeviceNodePcbMsg::getInstancePtr()
{
    if(!m_instance)
        m_instance = std::make_unique<DeviceNodePcbMsg>();
    return m_instance;
}
