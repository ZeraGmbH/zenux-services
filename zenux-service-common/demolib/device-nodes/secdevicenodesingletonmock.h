#ifndef SECDEVICENODESINGLETONMOCK_H
#define SECDEVICENODESINGLETONMOCK_H

#include <secdevicenodesingleton.h>

class SecDeviceNodeSingletonMock : public SecDeviceNodeSingleton
{
public:
    static void enableMock();
};

#endif // SECDEVICENODESINGLETONMOCK_H
