#include "pcbdevicenodemessagesingletonmock.h"
#include "pcbdevicenodemessagemock.h"

void PcbDeviceNodeMessageSingletonMock::enableMock()
{
    m_createFunction = []() {
        return std::make_unique<PcbDeviceNodeMessageMock>();
    };
}
