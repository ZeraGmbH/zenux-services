#include "singletondevicenodesec.h"
#include "devicenodesec.h"

AbstractDeviceNodeSecPtr SingletonDeviceNodeSec::m_instance;

AbstractDeviceNodeSecPtr SingletonDeviceNodeSec::getInstancePtr()
{
    if(!m_instance)
        m_instance = std::make_shared<DeviceNodeSec>();
    return m_instance;
}
