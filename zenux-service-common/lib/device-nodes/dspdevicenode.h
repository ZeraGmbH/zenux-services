#ifndef DSPDEVICENODE_H
#define DSPDEVICENODE_H

#include "dspdevicenodeaccessinterface.h"

class DspDeviceNode : public DspDeviceNodeInterface
{
public:
    static const int MAGIC_ID21262;
    static const int MAGIC_ID21362;

    // enum zum lesen von dsp port adressen über ioctl
    // Serial Port Interface,  Serial Interface, DSP Ctrl Register, what ?, the device name
    enum IOCTL {SPI, Serial, DSPCtrl, DSPStat, DSPCfg, VersionNr, MagicId};

    int open(QString devNodeFileName) override;
    void close() override;
    bool dspReset() override;
    bool dspBoot(QString bootFileName) override;
    int lseek(ulong adr) override;
    int write(const char* buf, int len) override;
    int read(char* buf, int len) override;
    void enableFasync() override;
    int dspRequestInt() override;
    int ioctlDspIoRead(unsigned long arg) override;
private:
    int ioctlDspReset();
    int ioctlDspBoot(const char* firmwareData);
    int m_devFileDescriptor = -1;
    QString m_devNodeFileName;
};

#endif // DSPDEVICENODE_H
