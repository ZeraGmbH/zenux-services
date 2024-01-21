#ifndef MOCKPCBDEVICENODECTRL_H
#define MOCKPCBDEVICENODECTRL_H

#include <abstractpcbctrldevicenode.h>

class MockPcbDeviceNodeCtrl : public AbstractPcbCtrlDeviceNode
{
public:
    int open(QString devNodeFileName) override;
    void close() override;
    void enableFasync() override;
};

#endif // MOCKPCBDEVICENODECTRL_H
