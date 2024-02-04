#include "factorydevicenodedsp.h"
#include "singletondevicenodedsp.h"

AbstractDspDeviceNodePtr FactoryDeviceNodeDsp::getDspDeviceNode()
{
    return SingletonDeviceNodeDsp::getInstancePtr();
}
