#ifndef TESTHOTPLUGCTRLFACTORYI2CCTRL_H
#define TESTHOTPLUGCTRLFACTORYI2CCTRL_H

#include "testfactoryi2cctrl.h"
#include "i2csettings.h"

class TestHotPlugCtrlFactoryI2cCtrl : public TestFactoryI2cCtrl
{
public:
    TestHotPlugCtrlFactoryI2cCtrl(I2cSettings *i2cSettings);
    I2cCtrlCommonInfoPtrUnique getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel = -1) override;

private:
    I2cSettings *m_i2cSettings;
};

#endif // TESTHOTPLUGCTRLFACTORYI2CCTRL_H
