#include "dspdevicenodesingleton.h"
#include "dspdevicenode.h"

AbstractDspDeviceNodePtr DspDeviceNodeSingleton::m_instance;

std::function<AbstractDspDeviceNodePtr()> DspDeviceNodeSingleton::m_createFunction = []() {
    return std::make_unique<DspDeviceNode>();
};

AbstractDspDeviceNode *DspDeviceNodeSingleton::getInstance()
{
    if(m_instance == nullptr)
        m_instance = m_createFunction();
    return m_instance.get();
}
