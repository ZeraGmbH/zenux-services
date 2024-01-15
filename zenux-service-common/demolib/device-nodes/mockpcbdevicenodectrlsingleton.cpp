#include "mockpcbdevicenodectrlsingleton.h"
#include "mockpcbdevicenodectrl.h"

void MockPcbDeviceNodeCtrlSingleton::enableMock()
{
    m_createFunction = []() {
        return std::make_unique<MockPcbDeviceNodeCtrl>();
    };
}
