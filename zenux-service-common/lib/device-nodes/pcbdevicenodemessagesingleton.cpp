#include "pcbdevicenodemessagesingleton.h"
#include "pcbdevicenodemessage.h"

PcbDeviceNodeMessageAbstractPtr PcbDeviceNodeMessageSingleton::m_instance;

std::function<PcbDeviceNodeMessageAbstractPtr()> PcbDeviceNodeMessageSingleton::m_createFunction = []() {
    return std::make_unique<PcbDeviceNodeMessage>();
};

PcbDeviceNodeMessageAbstract *PcbDeviceNodeMessageSingleton::getInstance()
{
    if(m_instance == nullptr)
        m_instance = m_createFunction();
    return m_instance.get();
}
