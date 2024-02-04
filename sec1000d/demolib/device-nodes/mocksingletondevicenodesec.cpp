#include "mocksingletondevicenodesec.h"
#include "mockdevicenodesec.h"

AbstractDeviceNodeSecPtr MockSingletonDeviceNodeSec::m_instance;

AbstractDeviceNodeSecPtr MockSingletonDeviceNodeSec::getInstancePtr()
{
    if(!m_instance)
        m_instance = std::make_unique<MockDeviceNodeSec>();
    return m_instance;
}
