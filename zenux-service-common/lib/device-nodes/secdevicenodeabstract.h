#ifndef SECDEVICENODEABSTRACT_H
#define SECDEVICENODEABSTRACT_H

#include <QString>
#include <memory>

class SecDeviceNodeAbstract
{
public:
    ~SecDeviceNodeAbstract() = default;
    virtual int open(QString devNodeFileName) = 0;
    virtual void close() = 0;

    virtual int lseek(ulong adr) = 0;
    virtual int write(const char* buf, int len) = 0;
    virtual int read(char* buf, int len) = 0;
    virtual void enableFasync() = 0;
};

typedef std::unique_ptr<SecDeviceNodeAbstract> SecDeviceNodeAbstractPtr;

#endif // SECDEVICENODEABSTRACT_H
