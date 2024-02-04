#ifndef MOCKSINGLETONDEVICENODESEC_H
#define MOCKSINGLETONDEVICENODESEC_H

#include "abstractdevicenodesec.h"

class MockSingletonDeviceNodeSec
{
public:
    static AbstractDeviceNodeSecPtr getInstancePtr();
private:
    static AbstractDeviceNodeSecPtr m_instance;
};

#endif // MOCKSINGLETONDEVICENODESEC_H
