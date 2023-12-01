#ifndef DSPDEVICENODEMOCK_H
#define DSPDEVICENODEMOCK_H

#include "dspdevicenodeaccessinterface.h"

class DspDeviceNodeMock : public DspDeviceNodeInterface
{
public:
    DspDeviceNodeMock(int dspMagicId);
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
    int m_dspMagicId;
};

#endif // DSPDEVICENODEMOCK_H