#ifndef PCBDEVICENODECTRLSINGLETON_H
#define PCBDEVICENODECTRLSINGLETON_H

#include "pcbdevicenodectrlinterface.h"
#include <functional>

class PcbDeviceNodeCtrlSingleton
{
public:
    static PcbDeviceNodeCtrlInterface *getInstance();
protected:
    static std::function<PcbDeviceNodeCtrlInterfaceUPtr()> m_createFunction;
private:
    static PcbDeviceNodeCtrlInterfaceUPtr m_instance;
};

#endif // PCBDEVICENODECTRLSINGLETON_H
