#include "mocksecdevicenode.h"

int MockSecDeviceNode::open(QString devNodeFileName)
{
    Q_UNUSED(devNodeFileName)
    return 0;
}

void MockSecDeviceNode::close()
{
}

int MockSecDeviceNode::lseek(ulong adr)
{
    Q_UNUSED(adr)
    return 0;
}

int MockSecDeviceNode::write(const char *buf, int len)
{
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return 0;
}

int MockSecDeviceNode::read(char *buf, int len)
{
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return 0;
}

void MockSecDeviceNode::enableFasync()
{
}
