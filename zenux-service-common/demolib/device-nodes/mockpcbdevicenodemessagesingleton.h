#ifndef MOCKPCBDEVICENODEMESSAGESINGLETON_H
#define MOCKPCBDEVICENODEMESSAGESINGLETON_H

#include <pcbdevicenodemessagesingleton.h>

class MockPcbDeviceNodeMessageSingleton : public PcbDeviceNodeMessageSingleton
{
public:
    static void enableMock();
};

#endif // MOCKPCBDEVICENODEMESSAGESINGLETON_H
