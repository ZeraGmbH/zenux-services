#ifndef DSPDEVICENODEMOCK_H
#define DSPDEVICENODEMOCK_H

#include "dspdevicenodeaccessinterface.h"

class DspDeviceNodeMock : public DspDeviceNodeInterface
{
public:
    int open(QString devNodeFileName) override;
    void close() override;
    int lseek(ulong adr) override;
    int write(const char* buf, int len) override;
    int read(char* buf, int len) override;
    void setFasync() override;
    int ioctlDspReset() override;
    int ioctlDspBoot(const char* firmwareData) override;
    int ioctlDspRequestInt() override;
    int ioctlDspIoRead(unsigned long arg) override;
};

#endif // DSPDEVICENODEMOCK_H
