#include "pcbdevicenodemessagesingleton.h"
#include "pcbdevicenodemessage.h"

AbstractPcbMsgDeviceNodePtr PcbDeviceNodeMessageSingleton::m_instance;

std::function<AbstractPcbMsgDeviceNodePtr()> PcbDeviceNodeMessageSingleton::m_createFunction = []() {
    return std::make_unique<PcbDeviceNodeMessage>();
};

AbstractPcbMsgDeviceNode *PcbDeviceNodeMessageSingleton::getInstance()
{
    if(m_instance == nullptr)
        m_instance = m_createFunction();
    return m_instance.get();
}
