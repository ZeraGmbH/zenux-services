#include "secdevicenodesingleton.h"
#include "secdevicenode.h"

AbstractSecDeviceNodePtr SecDeviceNodeSingleton::m_instance;

std::function<AbstractSecDeviceNodePtr()> SecDeviceNodeSingleton::m_createFunction = []() {
    return std::make_unique<SecDeviceNode>();
};

AbstractSecDeviceNode *SecDeviceNodeSingleton::getInstance()
{
    if(m_instance == nullptr)
        m_instance = m_createFunction();
    return m_instance.get();
}
