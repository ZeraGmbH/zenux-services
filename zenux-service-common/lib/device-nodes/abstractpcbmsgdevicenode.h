#ifndef ABSTRACTPCBMSGDEVICENODE_H
#define ABSTRACTPCBMSGDEVICENODE_H

#include <QString>
#include <memory>

class AbstractPcbMsgDeviceNode
{
public:
    virtual ~AbstractPcbMsgDeviceNode() = default;
    virtual int open(QString devNodeFileName) = 0;
    virtual void close() = 0;
    virtual void enableFasync() = 0;
};

typedef std::unique_ptr<AbstractPcbMsgDeviceNode> AbstractPcbMsgDeviceNodePtr;

#endif // ABSTRACTPCBMSGDEVICENODE_H
