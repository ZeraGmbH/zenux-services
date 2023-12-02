#include "dspdevicenodesingletonmock.h"
#include "dspdevicenodemock.h"

void DspDeviceNodeSingletonMock::enableMock(int dspMagicId)
{
    m_createFunction = [dspMagicId]() {
        return std::make_unique<DspDeviceNodeMock>(dspMagicId);
    };
}
