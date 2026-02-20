#include "demofactoryzdspsupport.h"
#include "demosingletondevicenodedsp.h"
#include "dspcompilerrawcollector.h"

DemoFactoryZdspSupport::DemoFactoryZdspSupport(int dspMagicId) :
    m_dspMagicId(dspMagicId)
{
}

AbstractDspDeviceNodePtr DemoFactoryZdspSupport::getDspDeviceNode()
{
    return DemoSingletonDeviceNodeDsp::getInstancePtr(m_dspMagicId);
}

AbstractDspCompilerRawCollectorPtr DemoFactoryZdspSupport::createDspCompilerRawCollector()
{
    return std::make_shared<DspCompilerRawCollector>();
}
