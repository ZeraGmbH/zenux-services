#ifndef PCBDEVICENODECTRLMOCK_H
#define PCBDEVICENODECTRLMOCK_H

#include <pcbdevicenodectrlinterface.h>

class PcbDeviceNodeCtrlMock : public PcbDeviceNodeCtrlInterface
{
public:
    int open(QString devNodeFileName) override;
    void close() override;
    void enableFasync() override;
};

#endif // PCBDEVICENODECTRLMOCK_H
