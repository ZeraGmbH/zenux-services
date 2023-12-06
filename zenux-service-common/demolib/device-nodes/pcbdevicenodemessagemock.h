#ifndef PCBDEVICENODEMESSAGEMOCK_H
#define PCBDEVICENODEMESSAGEMOCK_H

#include <pcbdevicenodemessageinterface.h>

class PcbDeviceNodeMessageMock : public PcbDeviceNodeMessageInterface
{
public:
    int open(QString devNodeFileName) override;
    void close() override;
    void enableFasync() override;
};

#endif // PCBDEVICENODEMESSAGEMOCK_H
