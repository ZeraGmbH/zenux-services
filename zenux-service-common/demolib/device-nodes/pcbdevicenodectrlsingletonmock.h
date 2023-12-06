#ifndef PCBDEVICENODECTRLSINGLETONMOCK_H
#define PCBDEVICENODECTRLSINGLETONMOCK_H

#include <pcbdevicenodectrlsingleton.h>

class PcbDeviceNodeCtrlSingletonMock : public PcbDeviceNodeCtrlSingleton
{
public:
    static void enableMock();
};

#endif // PCBDEVICENODECTRLSINGLETONMOCK_H
