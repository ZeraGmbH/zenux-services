#ifndef PCBDEVICENODECTRLSINGLETON_H
#define PCBDEVICENODECTRLSINGLETON_H

#include "abstractpcbctrldevicenode.h"
#include <functional>

class PcbDeviceNodeCtrlSingleton
{
public:
    static AbstractPcbCtrlDeviceNode *getInstance();
protected:
    static std::function<AbstractPcbCtrlDeviceNodePtr()> m_createFunction;
private:
    static AbstractPcbCtrlDeviceNodePtr m_instance;
};

#endif // PCBDEVICENODECTRLSINGLETON_H
