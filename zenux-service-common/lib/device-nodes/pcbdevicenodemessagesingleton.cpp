#include "pcbdevicenodemessagesingleton.h"
#include "pcbdevicenodemessage.h"

PcbDeviceNodeMessageInterfaceUPtr PcbDeviceNodeMessageSingleton::m_instance;

std::function<PcbDeviceNodeMessageInterfaceUPtr()> PcbDeviceNodeMessageSingleton::m_createFunction = []() {
    return std::make_unique<PcbDeviceNodeMessage>();
};

PcbDeviceNodeMessageInterface *PcbDeviceNodeMessageSingleton::getInstance()
{
    if(m_instance == nullptr)
        m_instance = m_createFunction();
    return m_instance.get();
}
