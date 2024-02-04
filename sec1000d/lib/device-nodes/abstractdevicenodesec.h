#ifndef ABSTRACTDEVICENODESEC_H
#define ABSTRACTDEVICENODESEC_H

#include <QString>
#include <memory>

class AbstractDeviceNodeSec
{
public:
    virtual ~AbstractDeviceNodeSec() = default;
    virtual int open(QString devNodeFileName) = 0;
    virtual void close() = 0;

    virtual int lseek(ulong adr) = 0;
    virtual int write(const char* buf, int len) = 0;
    virtual int read(char* buf, int len) = 0;
    virtual void enableFasync() = 0;
};

typedef std::shared_ptr<AbstractDeviceNodeSec> AbstractDeviceNodeSecPtr;

#endif // ABSTRACTDEVICENODESEC_H
