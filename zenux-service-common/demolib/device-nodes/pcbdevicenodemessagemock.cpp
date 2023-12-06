#include "pcbdevicenodemessagemock.h"

int PcbDeviceNodeMessageMock::open(QString devNodeFileName)
{
    Q_UNUSED(devNodeFileName)
    return 0;
}

void PcbDeviceNodeMessageMock::close()
{
}

void PcbDeviceNodeMessageMock::enableFasync()
{
}
