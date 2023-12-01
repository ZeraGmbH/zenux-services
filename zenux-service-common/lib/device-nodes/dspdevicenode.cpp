#include "dspdevicenode.h"
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

bool DspDeviceNode::dspBoot(QString bootFileName)
{
    // We should not report errors here but this is an exception
    // since there are two transactions
    QFile f (bootFileName);
    if (!f.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
        qCritical("Error opening dsp boot file: %s", qPrintable(bootFileName));
        return false;
    }
    QByteArray BootMem = f.readAll();
    f.close();

    int r = ioctlDspBoot(BootMem.data());
    if(r < 0)
        qCritical("Error %d booting dsp device: %s", r, qPrintable(m_devNodeFileName));
    return r >= 0;
}

bool DspDeviceNode::dspReset()
{
    return ioctlDspReset() >= 0;
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

// enum zum lesen von dsp port adressen über ioctl
// Serial Port Interface,  Serial Interface, DSP Ctrl Register, what ?, the device name
enum IOCTL {SPI, Serial, DSPCtrl, DSPStat, DSPCfg, VersionNr, MagicId};

int DspDeviceNode::dspGetMagicId()
{
    return ioctlDspIoRead(MagicId);
}

bool DspDeviceNode::dspIsRunning()
{
    constexpr int DSP_RUNNING = 0x80;
    int r = ioctlDspIoRead(DSPStat);
    return ((r & DSP_RUNNING) > 0);

}

int DspDeviceNode::lcaRawVersion()
{
    return ioctlDspIoRead(VersionNr);
}

int DspDeviceNode::ioctlDspIoRead(unsigned long arg)
{
    return ioctl(m_devFileDescriptor, IO_READ, arg);
}
