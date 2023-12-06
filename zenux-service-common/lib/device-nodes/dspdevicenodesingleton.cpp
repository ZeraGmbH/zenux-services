#include "dspdevicenodesingleton.h"
#include "dspdevicenode.h"

DspDeviceNodeInterfaceUPtr DspDeviceNodeSingleton::m_instance;

std::function<DspDeviceNodeInterfaceUPtr()> DspDeviceNodeSingleton::m_createFunction = []() {
    return std::make_unique<DspDeviceNode>();
};

DspDeviceNodeInterface *DspDeviceNodeSingleton::getInstance()
{
    if(m_instance == nullptr)
        m_instance = m_createFunction();
    return m_instance.get();
}
