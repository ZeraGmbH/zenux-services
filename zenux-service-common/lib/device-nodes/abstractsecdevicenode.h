#ifndef ABSTRACTSECDEVICENODE_H
#define ABSTRACTSECDEVICENODE_H

#include <QString>
#include <memory>

class AbstractSecDeviceNode
{
public:
    ~AbstractSecDeviceNode() = default;
    virtual int open(QString devNodeFileName) = 0;
    virtual void close() = 0;

    virtual int lseek(ulong adr) = 0;
    virtual int write(const char* buf, int len) = 0;
    virtual int read(char* buf, int len) = 0;
    virtual void enableFasync() = 0;
};

typedef std::unique_ptr<AbstractSecDeviceNode> AbstractSecDeviceNodePtr;

#endif // ABSTRACTSECDEVICENODE_H
