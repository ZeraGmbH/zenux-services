#include "secdevicenodesingleton.h"
#include "secdevicenode.h"

SecDeviceNodeInterfaceUPtr SecDeviceNodeSingleton::m_instance;

std::function<SecDeviceNodeInterfaceUPtr()> SecDeviceNodeSingleton::m_createFunction = []() {
    return std::make_unique<SecDeviceNode>();
};

SecDeviceNodeInterface *SecDeviceNodeSingleton::getInstance()
{
    if(m_instance == nullptr)
        m_instance = m_createFunction();
    return m_instance.get();
}