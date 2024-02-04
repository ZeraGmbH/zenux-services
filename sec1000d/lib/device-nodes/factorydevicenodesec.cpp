#include "factorydevicenodesec.h"
#include "singletondevicenodesec.h"

AbstractDeviceNodeSecPtr FactoryDeviceNodeSec::getSecDeviceNode()
{
    return SingletonDeviceNodeSec::getInstancePtr();
}
