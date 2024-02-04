#include "mocksingletondevicenodepcbmsg.h"
#include "mockdevicenodepcbmsg.h"

AbstractDeviceNodePcbMsgPtr MockSingletonDeviceNodePcbMsg::m_instance;

AbstractDeviceNodePcbMsgPtr MockSingletonDeviceNodePcbMsg::getInstancePtr()
{
    if(!m_instance)
        m_instance = std::make_unique<MockDeviceNodePcbMsg>();
    return m_instance;
}
