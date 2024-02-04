#ifndef FACTORYDEVICENODESEC_H
#define FACTORYDEVICENODESEC_H

#include "abstractfactorydevicenodesec.h"

class FactoryDeviceNodeSec : public AbstractFactoryDeviceNodeSec
{
public:
    AbstractDeviceNodeSecPtr getSecDeviceNode() override;
};

#endif // FACTORYDEVICENODESEC_H
