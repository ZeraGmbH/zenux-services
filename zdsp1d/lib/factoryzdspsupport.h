#ifndef FACTORYZDSPSUPPORT_H
#define FACTORYZDSPSUPPORT_H

#include "abstractfactoryzdspsupport.h"

class FactoryZdspSupport : public AbstractFactoryZdspSupport
{
public:
    AbstractDspDeviceNodePtr getDspDeviceNode() override;
    AbstractDspCompilerSupportPtr createDspCompilerSupport() override;
};

#endif // FACTORYZDSPSUPPORT_H
