#include "mockdspdevicenode.h"

MockDspDeviceNode::MockDspDeviceNode(int dspMagicId) :
    m_dspMagicId(dspMagicId)
{
}

int MockDspDeviceNode::open(QString devNodeFileName)
{
    Q_UNUSED(devNodeFileName)
    return 0;
}

void MockDspDeviceNode::close()
{
}

bool MockDspDeviceNode::dspReset()
{
    return true;
}

bool MockDspDeviceNode::dspBoot(QString bootFileName)
{
    Q_UNUSED(bootFileName)
    return true;
}

int MockDspDeviceNode::lseek(ulong adr)
{
    Q_UNUSED(adr)
    return 0;
}

bool MockDspDeviceNode::write(ulong adr, const char *buf, int len)
{
    Q_UNUSED(adr)
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return true;
}

int MockDspDeviceNode::read(char *buf, int len)
{
    Q_UNUSED(buf)
    Q_UNUSED(len)
    return 0;
}

void MockDspDeviceNode::enableFasync()
{
}

int MockDspDeviceNode::dspRequestInt()
{
    return 0;
}

int MockDspDeviceNode::dspGetMagicId()
{
    return m_dspMagicId;
}

bool MockDspDeviceNode::dspIsRunning()
{
    return true;
}

int MockDspDeviceNode::lcaRawVersion()
{
    return 0;
}
