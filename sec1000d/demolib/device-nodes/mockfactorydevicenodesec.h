#ifndef MOCKFACTORYDEVICENODESEC_H
#define MOCKFACTORYDEVICENODESEC_H

#include <abstractfactorydevicenodesec.h>

class MockFactoryDeviceNodeSec : public AbstractFactoryDeviceNodeSec
{
public:
    AbstractDeviceNodeSecPtr getSecDeviceNode() override;
};

#endif // MOCKFACTORYDEVICENODESEC_H
