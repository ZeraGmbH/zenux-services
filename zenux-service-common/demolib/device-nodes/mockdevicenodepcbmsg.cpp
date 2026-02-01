#include "mockdevicenodepcbmsg.h"

int MockDeviceNodePcbMsg::open(const QString &devNodeFileName)
{
    Q_UNUSED(devNodeFileName)
    return 0;
}

void MockDeviceNodePcbMsg::close()
{
}

void MockDeviceNodePcbMsg::enableFasync()
{
}
