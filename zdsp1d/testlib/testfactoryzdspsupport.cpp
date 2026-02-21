#include "testfactoryzdspsupport.h"
#include "testdspcompilersupport.h"
#include "testsingletondevicenodedsp.h"

TestFactoryZdspSupport::TestFactoryZdspSupport(int dspMagicId) :
    m_dspMagicId(dspMagicId)
{
}

AbstractDspDeviceNodePtr TestFactoryZdspSupport::getDspDeviceNode()
{
    return TestSingletonDeviceNodeDsp::getInstancePtr(m_dspMagicId);
}

AbstractDspCompilerSupportPtr TestFactoryZdspSupport::createDspCompilerSupport()
{
    return std::make_shared<TestDspCompilerSupport>();
}
