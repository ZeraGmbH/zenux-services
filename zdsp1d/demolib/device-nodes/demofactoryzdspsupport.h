#ifndef DEMOFACTORYZDSPSUPPORT_H
#define DEMOFACTORYZDSPSUPPORT_H

#include "abstractfactoryzdspsupport.h"
#include "devicenodedsp.h"

class DemoFactoryZdspSupport : public AbstractFactoryZdspSupport
{
public:
    DemoFactoryZdspSupport(int dspMagicId = DeviceNodeDsp::MAGIC_ID21362);
    AbstractDspDeviceNodePtr getDspDeviceNode() override;
    AbstractDspCompilerRawCollectorPtr createDspCompilerRawCollector() override;
private:
    int m_dspMagicId;
};

#endif // DEMOFACTORYZDSPSUPPORT_H
