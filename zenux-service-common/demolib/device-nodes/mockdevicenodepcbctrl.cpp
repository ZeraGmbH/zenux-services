#include "mockdevicenodepcbctrl.h"

int MockDeviceNodePcbCtrl::open(QString devNodeFileName)
{
    Q_UNUSED(devNodeFileName)
    return 0;
}

void MockDeviceNodePcbCtrl::close()
{
}

void MockDeviceNodePcbCtrl::enableFasync()
{
}
