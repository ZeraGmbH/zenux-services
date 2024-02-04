#ifndef FACTORYDEVICENODEDSP_H
#define FACTORYDEVICENODEDSP_H

#include "abstractfactorydevicenodedsp.h"

class FactoryDeviceNodeDsp : public AbstractFactoryDeviceNodeDsp
{
public:
    AbstractDspDeviceNodePtr getDspDeviceNode() override;
};

#endif // FACTORYDEVICENODEDSP_H
