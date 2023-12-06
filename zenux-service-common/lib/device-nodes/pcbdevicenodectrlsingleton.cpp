#include "pcbdevicenodectrlsingleton.h"
#include "pcbdevicenodectrl.h"

PcbDeviceNodeCtrlInterfaceUPtr PcbDeviceNodeCtrlSingleton::m_instance;

std::function<PcbDeviceNodeCtrlInterfaceUPtr()> PcbDeviceNodeCtrlSingleton::m_createFunction = []() {
    return std::make_unique<PcbDeviceNodeCtrl>();
};

PcbDeviceNodeCtrlInterface *PcbDeviceNodeCtrlSingleton::getInstance()
{
    if(m_instance == nullptr)
        m_instance = m_createFunction();
    return m_instance.get();
}
