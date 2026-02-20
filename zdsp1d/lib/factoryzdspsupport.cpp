#include "factoryzdspsupport.h"
#include "dspcompilerrawcollector.h"
#include "singletondevicenodedsp.h"

AbstractDspDeviceNodePtr FactoryZdspSupport::getDspDeviceNode()
{
    return SingletonDeviceNodeDsp::getInstancePtr();
}

AbstractDspCompilerRawCollectorPtr FactoryZdspSupport::createDspCompilerRawCollector()
{
    return std::make_shared<DspCompilerRawCollector>();
}
