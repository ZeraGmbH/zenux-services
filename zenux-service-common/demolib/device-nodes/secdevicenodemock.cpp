#include "secdevicenodemock.h"

int SecDeviceNodeMock::open(QString devNodeFileName)
{
    Q_UNUSED(devNodeFileName)
    return 0;
}

void SecDeviceNodeMock::close()
{
}

int SecDeviceNodeMock::lseek(ulong adr)
{
    Q_UNUSED(adr)
    return 0;
}

int SecDeviceNodeMock::write(const char *buf, int len)
{
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return 0;
}

int SecDeviceNodeMock::read(char *buf, int len)
{
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return 0;
}

void SecDeviceNodeMock::enableFasync()
{
}
