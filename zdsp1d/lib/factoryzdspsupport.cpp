#include "factoryzdspsupport.h"
#include "dspcompilersupport.h"
#include "singletondevicenodedsp.h"

AbstractDspDeviceNodePtr FactoryZdspSupport::getDspDeviceNode()
{
    return SingletonDeviceNodeDsp::getInstancePtr();
}

AbstractDspCompilerSupportPtr FactoryZdspSupport::createDspCompilerSupport()
{
    return std::make_shared<DspCompilerSupport>();
}
