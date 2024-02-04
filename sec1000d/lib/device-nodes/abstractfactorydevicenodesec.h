#ifndef ABSTRACTFACTORYDEVICENODESEC_H
#define ABSTRACTFACTORYDEVICENODESEC_H

#include "abstractdevicenodesec.h"

class AbstractFactoryDeviceNodeSec
{
public:
    virtual AbstractDeviceNodeSecPtr getSecDeviceNode() = 0;
};

typedef std::shared_ptr<AbstractFactoryDeviceNodeSec> AbstractFactoryDeviceNodeSecPtr;

#endif // ABSTRACTFACTORYDEVICENODESEC_H
