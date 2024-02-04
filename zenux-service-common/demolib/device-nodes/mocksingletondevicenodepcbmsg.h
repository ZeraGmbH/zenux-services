#ifndef MOCKSINGLETONDEVICENODEPCBMSG_H
#define MOCKSINGLETONDEVICENODEPCBMSG_H

#include "abstractdevicenodepcbmsg.h"

class MockSingletonDeviceNodePcbMsg
{
public:
    static AbstractDeviceNodePcbMsgPtr getInstancePtr();
private:
    static AbstractDeviceNodePcbMsgPtr m_instance;
};

#endif // MOCKSINGLETONDEVICENODEPCBMSG_H
