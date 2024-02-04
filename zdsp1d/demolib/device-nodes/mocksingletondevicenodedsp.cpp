#include "mocksingletondevicenodedsp.h"
#include "mockdevicenodedsp.h"

AbstractDspDeviceNodePtr MockSingletonDeviceNodeDsp::m_instanceMagicId21262;
AbstractDspDeviceNodePtr MockSingletonDeviceNodeDsp::m_instanceMagicId21362;

AbstractDspDeviceNodePtr MockSingletonDeviceNodeDsp::getInstancePtr(int dspMagicId)
{
    if(dspMagicId == DeviceNodeDsp::MAGIC_ID21262) {
        if(m_instanceMagicId21262 == nullptr)
            m_instanceMagicId21262 = std::make_shared<MockDeviceNodeDsp>(DeviceNodeDsp::MAGIC_ID21262);
        return m_instanceMagicId21262;
    }
    if(dspMagicId == DeviceNodeDsp::MAGIC_ID21362) {
        if(m_instanceMagicId21362 == nullptr)
            m_instanceMagicId21362 = std::make_shared<MockDeviceNodeDsp>(DeviceNodeDsp::MAGIC_ID21362);
        return m_instanceMagicId21362;
    }
    return nullptr;
}
