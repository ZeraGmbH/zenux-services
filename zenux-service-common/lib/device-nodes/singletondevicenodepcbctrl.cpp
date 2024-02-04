#include "singletondevicenodepcbctrl.h"
#include "devicenodepcbctrl.h"

AbstractDeviceNodePcbCtrlPtr SingletonDeviceNodePcbCtrl::m_instance;

AbstractDeviceNodePcbCtrlPtr SingletonDeviceNodePcbCtrl::getInstancePtr()
{
    if(!m_instance)
        m_instance = std::make_shared<DeviceNodePcbCtrl>();
    return m_instance;
}
