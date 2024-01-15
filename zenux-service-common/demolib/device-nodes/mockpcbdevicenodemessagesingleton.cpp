#include "mockpcbdevicenodemessagesingleton.h"
#include "mockpcbdevicenodemessage.h"

void MockPcbDeviceNodeMessageSingleton::enableMock()
{
    m_createFunction = []() {
        return std::make_unique<MockPcbDeviceNodeMessage>();
    };
}
