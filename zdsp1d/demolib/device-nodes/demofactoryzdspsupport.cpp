#include "demofactoryzdspsupport.h"
#include "demosingletondevicenodedsp.h"
#include "dspdspcompilersupport.h"

DemoFactoryZdspSupport::DemoFactoryZdspSupport(int dspMagicId) :
    m_dspMagicId(dspMagicId)
{
}

AbstractDspDeviceNodePtr DemoFactoryZdspSupport::getDspDeviceNode()
{
    return DemoSingletonDeviceNodeDsp::getInstancePtr(m_dspMagicId);
}

AbstractDspCompilerSupportPtr DemoFactoryZdspSupport::createDspCompilerSupport()
{
    return std::make_shared<DspDspCompilerSupport>();
}
