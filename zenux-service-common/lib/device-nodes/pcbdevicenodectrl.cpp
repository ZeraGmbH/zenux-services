#include "pcbdevicenodectrl.h"
#include <fcntl.h>
#include <unistd.h>

int PcbDeviceNodeCtrl::open(QString devNodeFileName)
{
    m_devNodeFileName = devNodeFileName;
    m_devFileDescriptor = ::open(devNodeFileName.toLatin1().data(), O_RDWR);
    if (m_devFileDescriptor  < 0)
        qCritical("Error opening pcb ctrl device: %s", qPrintable(m_devNodeFileName));
    return m_devFileDescriptor;
}

void PcbDeviceNodeCtrl::close()
{
    ::close(m_devFileDescriptor);
    m_devFileDescriptor = -1;
}

void PcbDeviceNodeCtrl::enableFasync()
{
    fcntl(m_devFileDescriptor, F_SETOWN, getpid()); // wir sind "besitzer" des device
    int oflags = fcntl(m_devFileDescriptor, F_GETFL);
    fcntl(m_devFileDescriptor, F_SETFL, oflags | FASYNC); // async. benachrichtung (sigio) einschalten
}
