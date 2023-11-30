#ifndef DSPDEVICENODEACCESSINTERFACE_H
#define DSPDEVICENODEACCESSINTERFACE_H

#include <QString>
#include <memory>

class DspDeviceNodeInterface
{
public:
    ~DspDeviceNodeInterface() = default;
    virtual int open(QString devNodeFileName) = 0;
    virtual void close() = 0;
    virtual int lseek(ulong adr) = 0;
    virtual int write(const char* buf, int len) = 0;
    virtual int read(char* buf, int len) = 0;
    virtual void setFasync() = 0;
    virtual int ioctlDspReset() = 0;
    virtual int ioctlDspBoot(const char* firmwareData) = 0;
    virtual int ioctlDspRequestInt() = 0;
    virtual int ioctlDspIoRead(unsigned long arg) = 0;

};

typedef std::unique_ptr<DspDeviceNodeInterface> DspDeviceNodeInterfaceUPtr;

#endif // DSPDEVICENODEACCESSINTERFACE_H
