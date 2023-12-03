#include "dspdevicenode.h"
#include <QFile>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

const int DspDeviceNode::MAGIC_ID21262 = 0xAA55BB44;
const int DspDeviceNode::MAGIC_ID21362 = 0xAA55CC33;

int DspDeviceNode::open(QString devNodeFileName)
{
    m_devNodeFileName = devNodeFileName;
    m_devFileDescriptor = ::open(devNodeFileName.toLatin1().data(), O_RDWR);
    if (m_devFileDescriptor  < 0)
        qCritical("Error opening dsp device: %s", qPrintable(m_devNodeFileName));
    return m_devFileDescriptor;
}

void DspDeviceNode::close()
{
    ::close(m_devFileDescriptor);
    m_devFileDescriptor = -1;
}

bool DspDeviceNode::dspBoot(QString bootFileName)
{
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
    int r = ::lseek(m_devFileDescriptor, adr, 0);
    if (r < 0)
        qCritical("Error positioning dsp device: %s", qPrintable(m_devNodeFileName));
    return r;
}

bool DspDeviceNode::write(ulong adr, const char *buf, int len)
{
    if(lseek(adr) >= 0 && _write(buf, len) >= 0)
        return true;
    return false;
}

int DspDeviceNode::_write(const char *buf, int len)
{
    int r = ::write(m_devFileDescriptor, buf, len);
    if (r < 0)
        qCritical("Error writing dsp device: %s", qPrintable(m_devNodeFileName));
    return r;
}

int DspDeviceNode::read(char *buf, int len)
{
    int r = ::read(m_devFileDescriptor, buf, len);
    if (r < 0)
        qCritical("Error reading dsp device: %s", qPrintable(m_devNodeFileName));
    return r;
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
    int rawLcaVersion = ioctlDspIoRead(VersionNr);
    if (rawLcaVersion < 0)
        qCritical("Error %d reading device version: %s", rawLcaVersion, qPrintable(m_devNodeFileName));
    return rawLcaVersion;
}

int DspDeviceNode::ioctlDspIoRead(unsigned long arg)
{
    return ioctl(m_devFileDescriptor, IO_READ, arg);
}
