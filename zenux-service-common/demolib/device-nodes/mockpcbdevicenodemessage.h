#ifndef MOCKPCBDEVICENODEMESSAGE_H
#define MOCKPCBDEVICENODEMESSAGE_H

#include <pcbdevicenodemessageabstract.h>

class MockPcbDeviceNodeMessage : public PcbDeviceNodeMessageAbstract
{
public:
    int open(QString devNodeFileName) override;
    void close() override;
    void enableFasync() override;
};

#endif // MOCKPCBDEVICENODEMESSAGE_H
