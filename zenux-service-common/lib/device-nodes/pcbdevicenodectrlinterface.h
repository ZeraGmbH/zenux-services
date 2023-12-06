#ifndef PCBDEVICENODECTRLINTERFACE_H
#define PCBDEVICENODECTRLINTERFACE_H

#include <QString>
#include <memory>

class PcbDeviceNodeCtrlInterface
{
public:
    virtual ~PcbDeviceNodeCtrlInterface() = default;
    virtual int open(QString devNodeFileName) = 0;
    virtual void close() = 0;
    virtual void enableFasync() = 0;
};

typedef std::unique_ptr<PcbDeviceNodeCtrlInterface> PcbDeviceNodeCtrlInterfaceUPtr;

#endif // PCBDEVICENODECTRLINTERFACE_H
