#include "secdevicenodesingletonmock.h"
#include "secdevicenodemock.h"

void SecDeviceNodeSingletonMock::enableMock()
{
    m_createFunction = []() {
        return std::make_unique<SecDeviceNodeMock>();
    };
}
