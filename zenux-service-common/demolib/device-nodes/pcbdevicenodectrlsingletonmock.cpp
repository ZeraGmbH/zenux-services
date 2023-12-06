#include "pcbdevicenodectrlsingletonmock.h"
#include "pcbdevicenodectrlmock.h"

void PcbDeviceNodeCtrlSingletonMock::enableMock()
{
    m_createFunction = []() {
        return std::make_unique<PcbDeviceNodeCtrlMock>();
    };
}
