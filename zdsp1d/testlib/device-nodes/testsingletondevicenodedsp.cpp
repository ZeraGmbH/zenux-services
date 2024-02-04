#include "testsingletondevicenodedsp.h"

TestDeviceNodeDspPtr TestSingletonDeviceNodeDsp::m_instanceMagicId21262;
TestDeviceNodeDspPtr TestSingletonDeviceNodeDsp::m_instanceMagicId21362;

AbstractDspDeviceNodePtr TestSingletonDeviceNodeDsp::getInstancePtr(int dspMagicId)
{
    return getInstancePtrTest(dspMagicId);
}

TestDeviceNodeDspPtr TestSingletonDeviceNodeDsp::getInstancePtrTest(int dspMagicId)
{
    if(dspMagicId == DeviceNodeDsp::MAGIC_ID21262) {
        if(m_instanceMagicId21262 == nullptr)
            m_instanceMagicId21262 = std::make_shared<TestDeviceNodeDsp>(DeviceNodeDsp::MAGIC_ID21262);
        return m_instanceMagicId21262;
    }
    if(dspMagicId == DeviceNodeDsp::MAGIC_ID21362) {
        if(m_instanceMagicId21362 == nullptr)
            m_instanceMagicId21362 = std::make_shared<TestDeviceNodeDsp>(DeviceNodeDsp::MAGIC_ID21362);
        return m_instanceMagicId21362;
    }
    return nullptr;
}
