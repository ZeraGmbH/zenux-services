#ifndef TESTSINGLETONDEVICENODEDSP_H
#define TESTSINGLETONDEVICENODEDSP_H

#include <devicenodedsp.h>

class TestSingletonDeviceNodeDsp
{
public:
    static AbstractDspDeviceNodePtr getInstancePtr(int dspMagicId);
private:
    static AbstractDspDeviceNodePtr m_instanceMagicId21262;
    static AbstractDspDeviceNodePtr m_instanceMagicId21362;
};

#endif // TESTSINGLETONDEVICENODEDSP_H
