#ifndef SECDEVICENODEINTERFACE_H
#define SECDEVICENODEINTERFACE_H

#include <QString>
#include <memory>

class SecDeviceNodeInterface
{
public:
    ~SecDeviceNodeInterface() = default;
    virtual int open(QString devNodeFileName) = 0;
    virtual void close() = 0;

    virtual int lseek(ulong adr) = 0;
    virtual int write(const char* buf, int len) = 0;
    virtual int read(char* buf, int len) = 0;
    virtual void enableFasync() = 0;
};

typedef std::unique_ptr<SecDeviceNodeInterface> SecDeviceNodeInterfaceUPtr;

#endif // SECDEVICENODEINTERFACE_H
