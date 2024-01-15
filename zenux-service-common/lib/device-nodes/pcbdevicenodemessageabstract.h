#ifndef PCBDEVICENODEMESSAGEABSTRACT_H
#define PCBDEVICENODEMESSAGEABSTRACT_H

#include <QString>
#include <memory>

class PcbDeviceNodeMessageAbstract
{
public:
    virtual ~PcbDeviceNodeMessageAbstract() = default;
    virtual int open(QString devNodeFileName) = 0;
    virtual void close() = 0;
    virtual void enableFasync() = 0;
};

typedef std::unique_ptr<PcbDeviceNodeMessageAbstract> PcbDeviceNodeMessageAbstractPtr;

#endif // PCBDEVICENODEMESSAGEABSTRACT_H
