#include "demodevicenodedsp.h"

DemoDeviceNodeDsp::DemoDeviceNodeDsp(int dspMagicId) :
    m_dspMagicId(dspMagicId)
{
}

int DemoDeviceNodeDsp::open(QString devNodeFileName)
{
    Q_UNUSED(devNodeFileName)
    return 0;
}

void DemoDeviceNodeDsp::close()
{
}

bool DemoDeviceNodeDsp::dspReset()
{
    return true;
}

bool DemoDeviceNodeDsp::dspBoot(QString bootFileName)
{
    Q_UNUSED(bootFileName)
    return true;
}

int DemoDeviceNodeDsp::lseek(ulong adr)
{
    Q_UNUSED(adr)
    return 0;
}

bool DemoDeviceNodeDsp::write(ulong adr, const char *buf, int len)
{
    Q_UNUSED(adr)
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return true;
}

int DemoDeviceNodeDsp::read(char *buf, int len)
{
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return 0;
}

void DemoDeviceNodeDsp::enableFasync()
{
}

int DemoDeviceNodeDsp::dspRequestInt()
{
    return 0;
}

int DemoDeviceNodeDsp::dspGetMagicId()
{
    return m_dspMagicId;
}

bool DemoDeviceNodeDsp::dspIsRunning()
{
    return true;
}

int DemoDeviceNodeDsp::lcaRawVersion()
{
    return 0;
}
