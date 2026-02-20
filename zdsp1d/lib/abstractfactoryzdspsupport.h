#ifndef ABSTRACTFACTORYZDSPSUPPORT_H
#define ABSTRACTFACTORYZDSPSUPPORT_H

#include "abstractdspcompilerrawcollector.h"
#include "abstractdspdevicenode.h"

class AbstractFactoryZdspSupport
{
public:
    virtual AbstractDspDeviceNodePtr getDspDeviceNode() = 0;
    virtual AbstractDspCompilerRawCollectorPtr createDspCompilerRawCollector() = 0;
};

typedef std::shared_ptr<AbstractFactoryZdspSupport> AbstractFactoryZdspSupportPtr;

#endif // ABSTRACTFACTORYZDSPSUPPORT_H
