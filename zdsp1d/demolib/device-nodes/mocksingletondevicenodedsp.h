#ifndef MOCKSINGLETONDEVICENODEDSP_H
#define MOCKSINGLETONDEVICENODEDSP_H

#include <devicenodedsp.h>

class MockSingletonDeviceNodeDsp
{
public:
    static AbstractDspDeviceNodePtr getInstancePtr(int dspMagicId);
private:
    static AbstractDspDeviceNodePtr m_instanceMagicId21262;
    static AbstractDspDeviceNodePtr m_instanceMagicId21362;
};

#endif // MOCKSINGLETONDEVICENODEDSP_H
