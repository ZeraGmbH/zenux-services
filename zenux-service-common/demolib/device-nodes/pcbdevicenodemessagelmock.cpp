#include "pcbdevicenodemessagelmock.h"
#include "pcbdevicenodemessagemock.h"

void PcbDeviceNodeMessagelMock::enableMock()
{
    m_createFunction = []() {
        return std::make_unique<PcbDeviceNodeMessageMock>();
    };
}
