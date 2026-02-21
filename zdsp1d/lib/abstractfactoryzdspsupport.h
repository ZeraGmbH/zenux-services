#ifndef ABSTRACTFACTORYZDSPSUPPORT_H
#define ABSTRACTFACTORYZDSPSUPPORT_H

#include "abstractdspcompilersupport.h"
#include "abstractdspdevicenode.h"

class AbstractFactoryZdspSupport
{
public:
    virtual AbstractDspDeviceNodePtr getDspDeviceNode() = 0;
    virtual AbstractDspCompilerSupportPtr createDspCompilerSupport() = 0;
};

typedef std::shared_ptr<AbstractFactoryZdspSupport> AbstractFactoryZdspSupportPtr;

#endif // ABSTRACTFACTORYZDSPSUPPORT_H
