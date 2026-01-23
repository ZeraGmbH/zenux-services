#ifndef MOCKDEVICENODEPCBCTRL_H
#define MOCKDEVICENODEPCBCTRL_H

#include <abstractdevicenodepcbctrl.h>

class MockDeviceNodePcbCtrl : public AbstractDeviceNodePcbCtrl
{
public:
    int open(QString devNodeFileName) override;
    void close() override;
    void enableFasync() override;
};

#endif // MOCKDEVICENODEPCBCTRL_H
