#ifndef DEMOSINGLETONDEVICENODEDSP_H
#define DEMOSINGLETONDEVICENODEDSP_H

#include <devicenodedsp.h>

class DemoSingletonDeviceNodeDsp
{
public:
    static AbstractDspDeviceNodePtr getInstancePtr(int dspMagicId);
private:
    static AbstractDspDeviceNodePtr m_instanceMagicId21262;
    static AbstractDspDeviceNodePtr m_instanceMagicId21362;
};

#endif // DEMOSINGLETONDEVICENODEDSP_H
