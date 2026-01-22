#ifndef ABSTRACTDEVICENODEPCBMSG_H
#define ABSTRACTDEVICENODEPCBMSG_H

#include <QString>
#include <memory>

class AbstractDeviceNodePcbMsg
{
public:
    virtual ~AbstractDeviceNodePcbMsg() = default;
    virtual int open(const QString &devNodeFileName) = 0;
    virtual void close() = 0;
    virtual void enableFasync() = 0;
};

typedef std::shared_ptr<AbstractDeviceNodePcbMsg> AbstractDeviceNodePcbMsgPtr;

#endif // ABSTRACTDEVICENODEPCBMSG_H
