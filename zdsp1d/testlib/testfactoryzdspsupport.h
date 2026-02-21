#ifndef TESTFACTORYZDSPSUPPORT_H
#define TESTFACTORYZDSPSUPPORT_H

#include <abstractfactoryzdspsupport.h>
#include "devicenodedsp.h"

class TestFactoryZdspSupport : public AbstractFactoryZdspSupport
{
public:
    TestFactoryZdspSupport(int dspMagicId = DeviceNodeDsp::MAGIC_ID21362);
    AbstractDspDeviceNodePtr getDspDeviceNode() override;
    AbstractDspCompilerSupportPtr createDspCompilerSupport() override;
private:
    int m_dspMagicId;
};

#endif // TESTFACTORYZDSPSUPPORT_H
