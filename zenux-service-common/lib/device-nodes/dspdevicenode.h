#ifndef DSPDEVICENODE_H
#define DSPDEVICENODE_H

#include "dspdevicenodeabstract.h"

class DspDeviceNode : public DspDeviceNodeAbstract
{
public:
    static const int MAGIC_ID21262;
    static const int MAGIC_ID21362;

    int open(QString devNodeFileName) override;
    void close() override;
    bool dspReset() override;
    bool dspBoot(QString bootFileName) override;
    int dspRequestInt() override;
    int dspGetMagicId() override;
    bool dspIsRunning() override;
    int lcaRawVersion() override;

    int lseek(ulong adr) override;
    bool write(ulong adr, const char* buf, int len) override;
    int read(char* buf, int len) override;
    void enableFasync() override;
private:
    int _write(const char* buf, int len);
    int ioctlDspIoRead(unsigned long arg);
    int ioctlDspReset();
    int ioctlDspBoot(const char* firmwareData);
    int m_devFileDescriptor = -1;
    QString m_devNodeFileName;
};

#endif // DSPDEVICENODE_H
