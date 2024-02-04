#include "demosingletondevicenodedsp.h"
#include "demodevicenodedsp.h"

AbstractDspDeviceNodePtr DemoSingletonDeviceNodeDsp::m_instanceMagicId21262;
AbstractDspDeviceNodePtr DemoSingletonDeviceNodeDsp::m_instanceMagicId21362;

AbstractDspDeviceNodePtr DemoSingletonDeviceNodeDsp::getInstancePtr(int dspMagicId)
{
    if(dspMagicId == DeviceNodeDsp::MAGIC_ID21262) {
        if(m_instanceMagicId21262 == nullptr)
            m_instanceMagicId21262 = std::make_shared<DemoDeviceNodeDsp>(DeviceNodeDsp::MAGIC_ID21262);
        return m_instanceMagicId21262;
    }
    if(dspMagicId == DeviceNodeDsp::MAGIC_ID21362) {
        if(m_instanceMagicId21362 == nullptr)
            m_instanceMagicId21362 = std::make_shared<DemoDeviceNodeDsp>(DeviceNodeDsp::MAGIC_ID21362);
        return m_instanceMagicId21362;
    }
    return nullptr;
}
