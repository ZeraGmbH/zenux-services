#ifndef ABSTRACTPCBCTRLDEVICENODE_H
#define ABSTRACTPCBCTRLDEVICENODE_H

#include <QString>
#include <memory>

class AbstractPcbCtrlDeviceNode
{
public:
    virtual ~AbstractPcbCtrlDeviceNode() = default;
    virtual int open(QString devNodeFileName) = 0;
    virtual void close() = 0;
    virtual void enableFasync() = 0;
};

typedef std::unique_ptr<AbstractPcbCtrlDeviceNode> AbstractPcbCtrlDeviceNodePtr;

#endif // ABSTRACTPCBCTRLDEVICENODE_H
