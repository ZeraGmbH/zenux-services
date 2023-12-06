#ifndef PCBDEVICENODEMESSAGEINTERFACE_H
#define PCBDEVICENODEMESSAGEINTERFACE_H

#include <QString>
#include <memory>

class PcbDeviceNodeMessageInterface
{
public:
    virtual ~PcbDeviceNodeMessageInterface() = default;
    virtual int open(QString devNodeFileName) = 0;
    virtual void close() = 0;
    virtual void enableFasync() = 0;
};

typedef std::unique_ptr<PcbDeviceNodeMessageInterface> PcbDeviceNodeMessageInterfaceUPtr;

#endif // PCBDEVICENODEMESSAGEINTERFACE_H
