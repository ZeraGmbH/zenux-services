#include "singletondevicenodedsp.h"
#include "devicenodedsp.h"

AbstractDspDeviceNodePtr SingletonDeviceNodeDsp::m_instance;

AbstractDspDeviceNodePtr SingletonDeviceNodeDsp::getInstancePtr()
{
    if(m_instance == nullptr)
        m_instance = std::make_shared<DeviceNodeDsp>();
    return m_instance;
}
