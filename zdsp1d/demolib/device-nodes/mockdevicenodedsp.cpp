#include "mockdevicenodedsp.h"

MockDeviceNodeDsp::MockDeviceNodeDsp(int dspMagicId) :
    m_dspMagicId(dspMagicId)
{
}

int MockDeviceNodeDsp::open(QString devNodeFileName)
{
    Q_UNUSED(devNodeFileName)
    return 0;
}

void MockDeviceNodeDsp::close()
{
}

bool MockDeviceNodeDsp::dspReset()
{
    return true;
}

bool MockDeviceNodeDsp::dspBoot(QString bootFileName)
{
    Q_UNUSED(bootFileName)
    return true;
}

int MockDeviceNodeDsp::lseek(ulong adr)
{
    Q_UNUSED(adr)
    return 0;
}

bool MockDeviceNodeDsp::write(ulong adr, const char *buf, int len)
{
    Q_UNUSED(adr)
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return true;
}

int MockDeviceNodeDsp::read(char *buf, int len)
{
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return 0;
}

void MockDeviceNodeDsp::enableFasync()
{
}

int MockDeviceNodeDsp::dspRequestInt()
{
    return 0;
}

int MockDeviceNodeDsp::dspGetMagicId()
{
    return m_dspMagicId;
}

bool MockDeviceNodeDsp::dspIsRunning()
{
    return true;
}

int MockDeviceNodeDsp::lcaRawVersion()
{
    return 0;
}
