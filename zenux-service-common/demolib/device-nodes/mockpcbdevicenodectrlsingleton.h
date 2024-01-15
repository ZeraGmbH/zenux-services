#ifndef MOCKPCBDEVICENODECTRLSINGLETON_H
#define MOCKPCBDEVICENODECTRLSINGLETON_H

#include <pcbdevicenodectrlsingleton.h>

class MockPcbDeviceNodeCtrlSingleton : public PcbDeviceNodeCtrlSingleton
{
public:
    static void enableMock();
};

#endif // MOCKPCBDEVICENODECTRLSINGLETON_H
