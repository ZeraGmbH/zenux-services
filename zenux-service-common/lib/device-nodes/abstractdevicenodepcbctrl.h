#ifndef ABSTRACTDEVICENODEPCBCTRL_H
#define ABSTRACTDEVICENODEPCBCTRL_H

#include <QString>
#include <memory>

class AbstractDeviceNodePcbCtrl
{
public:
    virtual ~AbstractDeviceNodePcbCtrl() = default;
    virtual int open(const QString &devNodeFileName) = 0;
    virtual void close() = 0;
    virtual void enableFasync() = 0;
};

typedef std::shared_ptr<AbstractDeviceNodePcbCtrl> AbstractDeviceNodePcbCtrlPtr;

#endif // ABSTRACTDEVICENODEPCBCTRL_H
