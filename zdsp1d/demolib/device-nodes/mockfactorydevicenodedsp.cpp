#include "mockfactorydevicenodedsp.h"
#include "mocksingletondevicenodedsp.h"

MockFactoryDeviceNodeDsp::MockFactoryDeviceNodeDsp(int dspMagicId) :
    m_dspMagicId(dspMagicId)
{
}

AbstractDspDeviceNodePtr MockFactoryDeviceNodeDsp::getDspDeviceNode()
{
    return MockSingletonDeviceNodeDsp::getInstancePtr(m_dspMagicId);
}
