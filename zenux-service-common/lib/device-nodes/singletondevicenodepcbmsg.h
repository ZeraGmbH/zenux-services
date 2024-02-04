#ifndef SINGLETONDEVICENODEPCBMSG_H
#define SINGLETONDEVICENODEPCBMSG_H

#include "abstractdevicenodepcbmsg.h"

class SingletonDeviceNodePcbMsg
{
public:
    static AbstractDeviceNodePcbMsgPtr getInstancePtr();
private:
    static AbstractDeviceNodePcbMsgPtr m_instance;
};

#endif // SINGLETONDEVICENODEPCBMSG_H
