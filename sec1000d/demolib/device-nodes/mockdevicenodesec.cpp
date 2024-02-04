#include "mockdevicenodesec.h"

int MockDeviceNodeSec::open(QString devNodeFileName)
{
    Q_UNUSED(devNodeFileName)
    return 0;
}

void MockDeviceNodeSec::close()
{
}

int MockDeviceNodeSec::lseek(ulong adr)
{
    Q_UNUSED(adr)
    return 0;
}

int MockDeviceNodeSec::write(const char *buf, int len)
{
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return 0;
}

int MockDeviceNodeSec::read(char *buf, int len)
{
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return 0;
}

void MockDeviceNodeSec::enableFasync()
{
}
