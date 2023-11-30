#include "dspdevicenodemock.h"
#include "dspdevicenode.h"
#include "zeraglobal.h"

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

bool DspDeviceNodeMock::bootDsp(QString bootFileName, QString &cmdAnswer)
{
    Q_UNUSED(bootFileName)
    cmdAnswer = ACKString;
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

void DspDeviceNodeMock::setFasync()
{
}

int DspDeviceNodeMock::ioctlDspReset()
{
    return 0;
}

int DspDeviceNodeMock::ioctlDspBoot(const char *firmwareData)
{
    Q_UNUSED(firmwareData)
    return 0;
}

int DspDeviceNodeMock::ioctlDspRequestInt()
{
    return 0;
}

int DspDeviceNodeMock::ioctlDspIoRead(unsigned long arg)
{
    if(arg == DspDeviceNode::MagicId)
        return m_dspMagicId;
    return 0;
}
