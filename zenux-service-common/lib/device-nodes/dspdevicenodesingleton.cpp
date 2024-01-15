#include "dspdevicenodesingleton.h"
#include "dspdevicenode.h"

DspDeviceNodeAbstractPtr DspDeviceNodeSingleton::m_instance;

std::function<DspDeviceNodeAbstractPtr()> DspDeviceNodeSingleton::m_createFunction = []() {
    return std::make_unique<DspDeviceNode>();
};

DspDeviceNodeAbstract *DspDeviceNodeSingleton::getInstance()
{
    if(m_instance == nullptr)
        m_instance = m_createFunction();
    return m_instance.get();
}
