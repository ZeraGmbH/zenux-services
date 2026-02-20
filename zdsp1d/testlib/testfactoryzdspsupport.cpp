#include "testfactoryzdspsupport.h"
#include "testdspcompilerrawcollector.h"
#include "testsingletondevicenodedsp.h"

TestFactoryZdspSupport::TestFactoryZdspSupport(int dspMagicId) :
    m_dspMagicId(dspMagicId)
{
}

AbstractDspDeviceNodePtr TestFactoryZdspSupport::getDspDeviceNode()
{
    return TestSingletonDeviceNodeDsp::getInstancePtr(m_dspMagicId);
}

AbstractDspCompilerRawCollectorPtr TestFactoryZdspSupport::createDspCompilerRawCollector()
{
    return std::make_shared<TestDspCompilerRawCollector>();
}
