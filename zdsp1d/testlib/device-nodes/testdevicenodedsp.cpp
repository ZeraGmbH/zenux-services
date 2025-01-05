#include "testdevicenodedsp.h"

TestDeviceNodeDsp::TestDeviceNodeDsp(int dspMagicId) :
    m_dspMagicId(dspMagicId)
{
}

bool TestDeviceNodeDsp::dspReset()
{
    emit sigIoOperation("dspReset");
    return true;
}

bool TestDeviceNodeDsp::dspBoot(QString bootFileName)
{
    emit sigIoOperation("dspBoot", bootFileName);
    return true;
}

void TestDeviceNodeDsp::enableFasync()
{
    emit sigIoOperation("enableFasync");
}

int TestDeviceNodeDsp::dspRequestInt()
{
    emit sigIoOperation("dspRequestInt");
    return 0;
}

int TestDeviceNodeDsp::dspGetMagicId()
{
    emit sigIoOperation("dspGetMagicId");
    return m_dspMagicId;
}

bool TestDeviceNodeDsp::dspIsRunning()
{
    emit sigIoOperation("dspIsRunning");
    return true;
}

int TestDeviceNodeDsp::lcaRawVersion()
{
    emit sigIoOperation("lcaRawVersion");
    return 0;
}

int TestDeviceNodeDsp::open(QString devNodeFileName)
{
    emit sigIoOperation("open", devNodeFileName);
    return 0;
}

void TestDeviceNodeDsp::close()
{
    emit sigIoOperation("close");
}

int TestDeviceNodeDsp::lseek(ulong adr)
{
    emit sigIoOperation("lseek", int(adr));
    return 0;
}

bool TestDeviceNodeDsp::write(ulong adr, const char *buf, int len)
{
    incWriteTransactionCount();
    QByteArray bytes(buf, len);
    emit sigIoOperation("write", int(adr), bytes, len);
    return true;
}

int TestDeviceNodeDsp::read(char *buf, int len)
{
    // make results at least reproducable
    for(int i=0; i<len; i++)
        *(buf+i) = 0;
    incReadTransactionCount();
    emit sigIoOperation("read", "buf", len);
    return 0;
}
