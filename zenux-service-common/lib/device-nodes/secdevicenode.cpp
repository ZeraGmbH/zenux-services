#include "secdevicenode.h"
#include <fcntl.h>
#include <unistd.h>

int SecDeviceNode::open(QString devNodeFileName)
{
    m_devNodeFileName = devNodeFileName;
    m_devFileDescriptor = ::open(devNodeFileName.toLatin1().data(), O_RDWR);
    if (m_devFileDescriptor  < 0)
        qCritical("Error opening dsp device: %s", qPrintable(m_devNodeFileName));
    return m_devFileDescriptor;
}

void SecDeviceNode::close()
{
    ::close(m_devFileDescriptor);
    m_devFileDescriptor = -1;
}

int SecDeviceNode::lseek(ulong adr)
{
    int r = ::lseek(m_devFileDescriptor, adr, 0);
    if (r < 0)
        qCritical("Error positioning sec device: %s", qPrintable(m_devNodeFileName));
    return r;
}

int SecDeviceNode::write(const char *buf, int len)
{
    int r = ::write(m_devFileDescriptor, buf, len);
    if (r < 0)
        qCritical("Error writing sec device: %s", qPrintable(m_devNodeFileName));
    return r;
}

int SecDeviceNode::read(char *buf, int len)
{
    int r = ::read(m_devFileDescriptor, buf, len);
    if (r < 0)
        qCritical("Error reading sec device: %s", qPrintable(m_devNodeFileName));
    return r;
}

void SecDeviceNode::enableFasync()
{
    fcntl(m_devFileDescriptor, F_SETOWN, getpid()); // wir sind "besitzer" des device
    int oflags = fcntl(m_devFileDescriptor, F_GETFL);
    fcntl(m_devFileDescriptor, F_SETFL, oflags | FASYNC); // async. benachrichtung (sigio) einschalten
}
