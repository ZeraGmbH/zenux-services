#include "mocksingletondevicenodepcbctrl.h"
#include "mockdevicenodepcbctrl.h"

AbstractDeviceNodePcbCtrlPtr MockSingletonDeviceNodePcbCtrl::m_instance;

AbstractDeviceNodePcbCtrlPtr MockSingletonDeviceNodePcbCtrl::getInstancePtr()
{
    if(!m_instance)
        m_instance = std::make_unique<MockDeviceNodePcbCtrl>();
    return m_instance;
}
