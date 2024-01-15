#include "mocksecdevicenodesingleton.h"
#include "mocksecdevicenode.h"

void MockSecDeviceNodeSingleton::enableMock()
{
    m_createFunction = []() {
        return std::make_unique<MockSecDeviceNode>();
    };
}
