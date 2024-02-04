#include "mockfactorydevicenodesec.h"
#include "mocksingletondevicenodesec.h"

AbstractDeviceNodeSecPtr MockFactoryDeviceNodeSec::getSecDeviceNode()
{
    return MockSingletonDeviceNodeSec::getInstancePtr();
}
