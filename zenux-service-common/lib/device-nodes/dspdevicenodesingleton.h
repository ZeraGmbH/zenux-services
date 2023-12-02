#ifndef DSPDEVICENODESINGLETON_H
#define DSPDEVICENODESINGLETON_H

#include "dspdevicenodeaccessinterface.h"
#include <functional>

class DspDeviceNodeSingleton
{
public:
    static DspDeviceNodeInterface *getInstance();
protected:
    static std::function<DspDeviceNodeInterfaceUPtr()> m_createFunction;
private:
    static DspDeviceNodeInterfaceUPtr m_instance;
};

#endif // DSPDEVICENODESINGLETON_H
