#ifndef MOCKFACTORYDEVICENODEDSP_H
#define MOCKFACTORYDEVICENODEDSP_H

#include <abstractfactorydevicenodedsp.h>

class MockFactoryDeviceNodeDsp : public AbstractFactoryDeviceNodeDsp
{
public:
    MockFactoryDeviceNodeDsp(int dspMagicId);
    AbstractDspDeviceNodePtr getDspDeviceNode() override;
private:
    int m_dspMagicId;
};

#endif // MOCKFACTORYDEVICENODEDSP_H
