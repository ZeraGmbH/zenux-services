#include "demofactorydevicenodedsp.h"
#include "demosingletondevicenodedsp.h"

DemoFactoryDeviceNodeDsp::DemoFactoryDeviceNodeDsp(int dspMagicId) :
    m_dspMagicId(dspMagicId)
{
}

AbstractDspDeviceNodePtr DemoFactoryDeviceNodeDsp::getDspDeviceNode()
{
    return DemoSingletonDeviceNodeDsp::getInstancePtr(m_dspMagicId);
}
