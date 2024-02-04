#ifndef SINGLETONDEVICENODEDSP_H
#define SINGLETONDEVICENODEDSP_H

#include "abstractdspdevicenode.h"

class SingletonDeviceNodeDsp
{
public:
    static AbstractDspDeviceNodePtr getInstancePtr();
private:
    static AbstractDspDeviceNodePtr m_instance;
};

#endif // SINGLETONDEVICENODEDSP_H
