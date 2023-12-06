#ifndef PCBDEVICENODEMESSAGESINGLETONMOCK_H
#define PCBDEVICENODEMESSAGESINGLETONMOCK_H

#include <pcbdevicenodemessagesingleton.h>

class PcbDeviceNodeMessageSingletonMock : public PcbDeviceNodeMessageSingleton
{
public:
    static void enableMock();
};

#endif // PCBDEVICENODEMESSAGESINGLETONMOCK_H
