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
    virtual bool dspReset() = 0;
    virtual bool dspBoot(QString bootFileName, QString &cmdAnswer) = 0;
    virtual int lseek(ulong adr) = 0;
    virtual int write(const char* buf, int len) = 0;
    virtual int read(char* buf, int len) = 0;
    virtual void enableFasync() = 0;
    virtual int dspRequestInt() = 0;
    virtual int ioctlDspIoRead(unsigned long arg) = 0;

};

typedef std::unique_ptr<DspDeviceNodeInterface> DspDeviceNodeInterfaceUPtr;

#endif // DSPDEVICENODEACCESSINTERFACE_H
