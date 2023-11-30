#include "dspdevicenode.h"
#include "zeraglobal.h"
#include <QFile>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

const int DspDeviceNode::MAGIC_ID21262 = 0xAA55BB44;
const int DspDeviceNode::MAGIC_ID21362 = 0xAA55CC33;

int DspDeviceNode::open(QString devNodeFileName)
{
    m_devFileDescriptor = ::open(devNodeFileName.toLatin1().data(), O_RDWR);
    m_devNodeFileName = devNodeFileName;
    return m_devFileDescriptor;
}

void DspDeviceNode::close()
{
    ::close(m_devFileDescriptor);
    m_devFileDescriptor = -1;
}

bool DspDeviceNode::dspBoot(QString bootFileName, QString &cmdAnswer)
{
    QFile f (bootFileName);
    if (!f.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
        qWarning("error opening dsp boot file: %s", qPrintable(bootFileName));
        cmdAnswer = ERRPATHString;
        return false;
    }
    QByteArray BootMem = f.readAll();
    f.close();
    int r = ioctlDspBoot(BootMem.data()); // und booten
    if ( r < 0 ) {
        qWarning("error %d booting dsp device: %s", r, qPrintable(m_devNodeFileName));
        cmdAnswer = ERREXECString; // fehler bei der ausführung
        return false;
    }
    cmdAnswer = ACKString;
    return true;
}

bool DspDeviceNode::dspReset(QString &cmdAnswer)
{
    int r = ioctlDspReset(); // und reset
    if ( r < 0 ) {
        qWarning("error %d reset dsp device: %s", r, qPrintable(m_devNodeFileName));
        cmdAnswer = ERREXECString; // fehler bei der ausführung
        return false;
    }
    cmdAnswer = ACKString;
    return true;
}

int DspDeviceNode::lseek(ulong adr)
{
    return ::lseek(m_devFileDescriptor, adr, 0);
}

int DspDeviceNode::write(const char *buf, int len)
{
    return ::write(m_devFileDescriptor, buf, len);
}

int DspDeviceNode::read(char *buf, int len)
{
    return ::read(m_devFileDescriptor, buf, len);
}

void DspDeviceNode::enableFasync()
{
    fcntl(m_devFileDescriptor, F_SETOWN, getpid()); // wir sind "besitzer" des device
    int oflags = fcntl(m_devFileDescriptor, F_GETFL);
    fcntl(m_devFileDescriptor, F_SETFL, oflags | FASYNC); // async. benachrichtung (sigio) einschalten
}


#define ADSP_IOC_MAGIC 'a'
/* ioctl commands */
#define ADSP_RESET _IOR(ADSP_IOC_MAGIC, 1, char*)
#define ADSP_BOOT _IOR(ADSP_IOC_MAGIC, 2, char*)
#define ADSP_INT_REQ _IOR(ADSP_IOC_MAGIC, 3, char*)
#define IO_READ _IOR(ADSP_IOC_MAGIC, 6, char*)

int DspDeviceNode::ioctlDspReset()
{
    return ioctl(m_devFileDescriptor, ADSP_RESET);
}

int DspDeviceNode::ioctlDspBoot(const char *firmwareData)
{
    return ioctl(m_devFileDescriptor, ADSP_BOOT, firmwareData);
}

int DspDeviceNode::dspRequestInt()
{
    return ioctl(m_devFileDescriptor, ADSP_INT_REQ);
}

int DspDeviceNode::ioctlDspIoRead(unsigned long arg)
{
    return ioctl(m_devFileDescriptor, IO_READ, arg);
}
