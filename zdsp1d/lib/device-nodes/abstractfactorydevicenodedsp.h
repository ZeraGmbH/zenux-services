#ifndef ABSTRACTFACTORYDEVICENODEDSP_H
#define ABSTRACTFACTORYDEVICENODEDSP_H

#include "abstractdspdevicenode.h"

class AbstractFactoryDeviceNodeDsp
{
public:
    virtual AbstractDspDeviceNodePtr getDspDeviceNode() = 0;
};

typedef std::shared_ptr<AbstractFactoryDeviceNodeDsp> AbstractFactoryDeviceNodeDspPtr;

#endif // ABSTRACTFACTORYDEVICENODEDSP_H
