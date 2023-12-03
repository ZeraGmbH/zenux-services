#include "dspdevicenodemock.h"

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

bool DspDeviceNodeMock::write(ulong adr, const char *buf, int len)
{
    Q_UNUSED(adr)
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return true;
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

int DspDeviceNodeMock::dspGetMagicId()
{
    return m_dspMagicId;
}

bool DspDeviceNodeMock::dspIsRunning()
{
    return true;
}

int DspDeviceNodeMock::lcaRawVersion()
{
    return 0;
}
