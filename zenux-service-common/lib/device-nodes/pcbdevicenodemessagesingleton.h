#ifndef PCBDEVICENODEMESSAGESINGLETON_H
#define PCBDEVICENODEMESSAGESINGLETON_H

#include "pcbdevicenodemessageinterface.h"
#include <functional>

class PcbDeviceNodeMessageSingleton
{
public:
    static PcbDeviceNodeMessageInterface *getInstance();
protected:
    static std::function<PcbDeviceNodeMessageInterfaceUPtr()> m_createFunction;
private:
    static PcbDeviceNodeMessageInterfaceUPtr m_instance;
};

#endif // PCBDEVICENODEMESSAGESINGLETON_H
