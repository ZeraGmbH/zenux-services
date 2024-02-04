#ifndef TESTSINGLETONDEVICENODEDSP_H
#define TESTSINGLETONDEVICENODEDSP_H

#include "testdevicenodedsp.h"
#include <devicenodedsp.h>

class TestSingletonDeviceNodeDsp
{
public:
    static AbstractDspDeviceNodePtr getInstancePtr(int dspMagicId);
    static TestDeviceNodeDspPtr getInstancePtrTest(int dspMagicId  = DeviceNodeDsp::MAGIC_ID21362);
private:
    static TestDeviceNodeDspPtr m_instanceMagicId21262;
    static TestDeviceNodeDspPtr m_instanceMagicId21362;
};

#endif // TESTSINGLETONDEVICENODEDSP_H
