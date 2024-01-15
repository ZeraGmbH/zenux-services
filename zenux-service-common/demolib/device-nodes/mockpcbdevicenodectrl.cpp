#include "mockpcbdevicenodectrl.h"

int MockPcbDeviceNodeCtrl::open(QString devNodeFileName)
{
    Q_UNUSED(devNodeFileName)
    return 0;
}

void MockPcbDeviceNodeCtrl::close()
{
}

void MockPcbDeviceNodeCtrl::enableFasync()
{
}
