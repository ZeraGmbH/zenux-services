#include "testfactorydevicenodedsp.h"
#include "testsingletondevicenodedsp.h"

TestFactoryDeviceNodeDsp::TestFactoryDeviceNodeDsp(int dspMagicId) :
    m_dspMagicId(dspMagicId)
{
}

AbstractDspDeviceNodePtr TestFactoryDeviceNodeDsp::getDspDeviceNode()
{
    return TestSingletonDeviceNodeDsp::getInstancePtr(m_dspMagicId);
}
