#include "pcbdevicenodectrlmock.h"

int PcbDeviceNodeCtrlMock::open(QString devNodeFileName)
{
    Q_UNUSED(devNodeFileName)
    return 0;
}

void PcbDeviceNodeCtrlMock::close()
{
}

void PcbDeviceNodeCtrlMock::enableFasync()
{
}
