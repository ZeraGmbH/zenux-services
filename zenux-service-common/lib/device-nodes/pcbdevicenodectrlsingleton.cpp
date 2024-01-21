#include "pcbdevicenodectrlsingleton.h"
#include "pcbdevicenodectrl.h"

AbstractPcbCtrlDeviceNodePtr PcbDeviceNodeCtrlSingleton::m_instance;

std::function<AbstractPcbCtrlDeviceNodePtr()> PcbDeviceNodeCtrlSingleton::m_createFunction = []() {
    return std::make_unique<PcbDeviceNodeCtrl>();
};

AbstractPcbCtrlDeviceNode *PcbDeviceNodeCtrlSingleton::getInstance()
{
    if(m_instance == nullptr)
        m_instance = m_createFunction();
    return m_instance.get();
}
