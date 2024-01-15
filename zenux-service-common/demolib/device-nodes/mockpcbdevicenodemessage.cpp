#include "mockpcbdevicenodemessage.h"

int MockPcbDeviceNodeMessage::open(QString devNodeFileName)
{
    Q_UNUSED(devNodeFileName)
    return 0;
}

void MockPcbDeviceNodeMessage::close()
{
}

void MockPcbDeviceNodeMessage::enableFasync()
{
}
