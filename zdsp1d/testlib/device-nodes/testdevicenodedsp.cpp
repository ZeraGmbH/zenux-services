#include "testdevicenodedsp.h"

TestDeviceNodeDsp::TestDeviceNodeDsp(int dspMagicId) :
    m_dspMagicId(dspMagicId)
{
}

int TestDeviceNodeDsp::open(QString devNodeFileName)
{
    Q_UNUSED(devNodeFileName)
    return 0;
}

void TestDeviceNodeDsp::close()
{
}

bool TestDeviceNodeDsp::dspReset()
{
    return true;
}

bool TestDeviceNodeDsp::dspBoot(QString bootFileName)
{
    Q_UNUSED(bootFileName)
    return true;
}

int TestDeviceNodeDsp::lseek(ulong adr)
{
    Q_UNUSED(adr)
    return 0;
}

bool TestDeviceNodeDsp::write(ulong adr, const char *buf, int len)
{
    Q_UNUSED(adr)
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return true;
}

int TestDeviceNodeDsp::read(char *buf, int len)
{
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return 0;
}

void TestDeviceNodeDsp::enableFasync()
{
}

int TestDeviceNodeDsp::dspRequestInt()
{
    return 0;
}

int TestDeviceNodeDsp::dspGetMagicId()
{
    return m_dspMagicId;
}

bool TestDeviceNodeDsp::dspIsRunning()
{
    return true;
}

int TestDeviceNodeDsp::lcaRawVersion()
{
    return 0;
}
