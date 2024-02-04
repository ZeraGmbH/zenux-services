#ifndef SINGLETONDEVICENODESEC_H
#define SINGLETONDEVICENODESEC_H

#include "abstractdevicenodesec.h"

class SingletonDeviceNodeSec
{
public:
    static AbstractDeviceNodeSecPtr getInstancePtr();
private:
    static AbstractDeviceNodeSecPtr m_instance;
};

#endif // SINGLETONDEVICENODESEC_H
