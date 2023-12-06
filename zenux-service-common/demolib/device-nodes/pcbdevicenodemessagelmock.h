#ifndef PCBDEVICENODEMESSAGELMOCK_H
#define PCBDEVICENODEMESSAGELMOCK_H

#include "pcbdevicenodemessagesingleton.h"

class PcbDeviceNodeMessagelMock : public PcbDeviceNodeMessageSingleton
{
public:
    static void enableMock();
};

#endif // PCBDEVICENODEMESSAGELMOCK_H
