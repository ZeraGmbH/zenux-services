#include "factoryzdspsupport.h"
#include "dspdspcompilersupport.h"
#include "singletondevicenodedsp.h"

AbstractDspDeviceNodePtr FactoryZdspSupport::getDspDeviceNode()
{
    return SingletonDeviceNodeDsp::getInstancePtr();
}

AbstractDspCompilerSupportPtr FactoryZdspSupport::createDspCompilerSupport()
{
    return std::make_shared<DspDspCompilerSupport>();
}
