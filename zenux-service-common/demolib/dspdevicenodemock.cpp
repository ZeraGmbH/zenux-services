#include "dspdevicenodemock.h"
#include "dspdevicenode.h"

DspDeviceNodeMock::DspDeviceNodeMock(int dspMagicId) :
    m_dspMagicId(dspMagicId)
{
}

int DspDeviceNodeMock::open(QString devNodeFileName)
{
    Q_UNUSED(devNodeFileName)
    return 0;
}

void DspDeviceNodeMock::close()
{
}

bool DspDeviceNodeMock::dspReset()
{
    return true;
}

bool DspDeviceNodeMock::dspBoot(QString bootFileName)
{
    Q_UNUSED(bootFileName)
    return true;
}

int DspDeviceNodeMock::lseek(ulong adr)
{
    Q_UNUSED(adr)
    return 0;
}

int DspDeviceNodeMock::write(const char *buf, int len)
{
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return 0;
}

int DspDeviceNodeMock::read(char *buf, int len)
{
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return 0;
}

void DspDeviceNodeMock::enableFasync()
{
}

int DspDeviceNodeMock::dspRequestInt()
{
    return 0;
}

int DspDeviceNodeMock::ioctlDspIoRead(unsigned long arg)
{
    if(arg == DspDeviceNode::MagicId)
        return m_dspMagicId;
    return 0;
}