#include "mockdspdevicenodesingleton.h"
#include "mockdspdevicenode.h"

void MockDspDeviceNodeSingleton::enableMock(int dspMagicId)
{
    m_createFunction = [dspMagicId]() {
        return std::make_unique<MockDspDeviceNode>(dspMagicId);
    };
}
