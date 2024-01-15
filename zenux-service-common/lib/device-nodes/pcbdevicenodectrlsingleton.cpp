#include "pcbdevicenodectrlsingleton.h"
#include "pcbdevicenodectrl.h"

PcbDeviceNodeCtrlAbstractPtr PcbDeviceNodeCtrlSingleton::m_instance;

std::function<PcbDeviceNodeCtrlAbstractPtr()> PcbDeviceNodeCtrlSingleton::m_createFunction = []() {
    return std::make_unique<PcbDeviceNodeCtrl>();
};

PcbDeviceNodeCtrlAbstract *PcbDeviceNodeCtrlSingleton::getInstance()
{
    if(m_instance == nullptr)
        m_instance = m_createFunction();
    return m_instance.get();
}
