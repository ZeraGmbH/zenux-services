#ifndef PCBDEVICENODECTRLSINGLETON_H
#define PCBDEVICENODECTRLSINGLETON_H

#include "pcbdevicenodectrlabstract.h"
#include <functional>

class PcbDeviceNodeCtrlSingleton
{
public:
    static PcbDeviceNodeCtrlAbstract *getInstance();
protected:
    static std::function<PcbDeviceNodeCtrlAbstractPtr()> m_createFunction;
private:
    static PcbDeviceNodeCtrlAbstractPtr m_instance;
};

#endif // PCBDEVICENODECTRLSINGLETON_H
