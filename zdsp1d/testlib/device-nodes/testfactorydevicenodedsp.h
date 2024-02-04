#ifndef TESTFACTORYDEVICENODEDSP_H
#define TESTFACTORYDEVICENODEDSP_H

#include <abstractfactorydevicenodedsp.h>
#include "devicenodedsp.h"

class TestFactoryDeviceNodeDsp : public AbstractFactoryDeviceNodeDsp
{
public:
    TestFactoryDeviceNodeDsp(int dspMagicId = DeviceNodeDsp::MAGIC_ID21362);
    AbstractDspDeviceNodePtr getDspDeviceNode() override;
private:
    int m_dspMagicId;
};

#endif // TESTFACTORYDEVICENODEDSP_H
