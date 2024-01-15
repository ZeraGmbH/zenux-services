#ifndef DSPDEVICENODESINGLETON_H
#define DSPDEVICENODESINGLETON_H

#include "dspdevicenodeabstract.h"
#include <functional>

class DspDeviceNodeSingleton
{
public:
    static DspDeviceNodeAbstract *getInstance();
protected:
    static std::function<DspDeviceNodeAbstractPtr()> m_createFunction;
private:
    static DspDeviceNodeAbstractPtr m_instance;
};

#endif // DSPDEVICENODESINGLETON_H
