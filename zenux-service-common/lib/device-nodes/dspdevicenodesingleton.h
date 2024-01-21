#ifndef DSPDEVICENODESINGLETON_H
#define DSPDEVICENODESINGLETON_H

#include "abstractdspdevicenode.h"
#include <functional>

class DspDeviceNodeSingleton
{
public:
    static AbstractDspDeviceNode *getInstance();
protected:
    static std::function<AbstractDspDeviceNodePtr()> m_createFunction;
private:
    static AbstractDspDeviceNodePtr m_instance;
};

#endif // DSPDEVICENODESINGLETON_H
