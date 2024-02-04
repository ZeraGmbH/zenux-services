#ifndef DEMOFACTORYDEVICENODEDSP_H
#define DEMOFACTORYDEVICENODEDSP_H

#include "abstractfactorydevicenodedsp.h"
#include "devicenodedsp.h"

class DemoFactoryDeviceNodeDsp : public AbstractFactoryDeviceNodeDsp
{
public:
    DemoFactoryDeviceNodeDsp(int dspMagicId = DeviceNodeDsp::MAGIC_ID21362);
    AbstractDspDeviceNodePtr getDspDeviceNode() override;
private:
    int m_dspMagicId;
};

#endif // DEMOFACTORYDEVICENODEDSP_H
