#ifndef PCBDEVICENODEMESSAGESINGLETON_H
#define PCBDEVICENODEMESSAGESINGLETON_H

#include "pcbdevicenodemessageabstract.h"
#include <functional>

class PcbDeviceNodeMessageSingleton
{
public:
    static PcbDeviceNodeMessageAbstract *getInstance();
protected:
    static std::function<PcbDeviceNodeMessageAbstractPtr()> m_createFunction;
private:
    static PcbDeviceNodeMessageAbstractPtr m_instance;
};

#endif // PCBDEVICENODEMESSAGESINGLETON_H
