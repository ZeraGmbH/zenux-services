#ifndef PCBDEVICENODEMESSAGESINGLETON_H
#define PCBDEVICENODEMESSAGESINGLETON_H

#include "abstractpcbmsgdevicenode.h"
#include <functional>

class PcbDeviceNodeMessageSingleton
{
public:
    static AbstractPcbMsgDeviceNode *getInstance();
protected:
    static std::function<AbstractPcbMsgDeviceNodePtr()> m_createFunction;
private:
    static AbstractPcbMsgDeviceNodePtr m_instance;
};

#endif // PCBDEVICENODEMESSAGESINGLETON_H
