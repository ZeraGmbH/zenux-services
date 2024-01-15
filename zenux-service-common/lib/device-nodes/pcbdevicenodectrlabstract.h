#ifndef PCBDEVICENODECTRLABSTRACT_H
#define PCBDEVICENODECTRLABSTRACT_H

#include <QString>
#include <memory>

class PcbDeviceNodeCtrlAbstract
{
public:
    virtual ~PcbDeviceNodeCtrlAbstract() = default;
    virtual int open(QString devNodeFileName) = 0;
    virtual void close() = 0;
    virtual void enableFasync() = 0;
};

typedef std::unique_ptr<PcbDeviceNodeCtrlAbstract> PcbDeviceNodeCtrlAbstractPtr;

#endif // PCBDEVICENODECTRLABSTRACT_H
