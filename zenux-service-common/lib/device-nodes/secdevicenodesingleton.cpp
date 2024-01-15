#include "secdevicenodesingleton.h"
#include "secdevicenode.h"

SecDeviceNodeAbstractPtr SecDeviceNodeSingleton::m_instance;

std::function<SecDeviceNodeAbstractPtr()> SecDeviceNodeSingleton::m_createFunction = []() {
    return std::make_unique<SecDeviceNode>();
};

SecDeviceNodeAbstract *SecDeviceNodeSingleton::getInstance()
{
    if(m_instance == nullptr)
        m_instance = m_createFunction();
    return m_instance.get();
}
