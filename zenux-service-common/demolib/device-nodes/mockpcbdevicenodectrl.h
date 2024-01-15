#ifndef MOCKPCBDEVICENODECTRL_H
#define MOCKPCBDEVICENODECTRL_H

#include <pcbdevicenodectrlabstract.h>

class MockPcbDeviceNodeCtrl : public PcbDeviceNodeCtrlAbstract
{
public:
    int open(QString devNodeFileName) override;
    void close() override;
    void enableFasync() override;
};

#endif // MOCKPCBDEVICENODECTRL_H
