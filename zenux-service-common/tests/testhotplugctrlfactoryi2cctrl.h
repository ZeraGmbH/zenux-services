#ifndef TESTHOTPLUGCTRLFACTORYI2CCTRL_H
#define TESTHOTPLUGCTRLFACTORYI2CCTRL_H

#include "sensesettings.h"
#include "testfactoryi2cctrl.h"
#include "i2csettings.h"

class TestHotPlugCtrlFactoryI2cCtrl : public TestFactoryI2cCtrl
{
public:
    TestHotPlugCtrlFactoryI2cCtrl(I2cSettingsPtr i2cSettings,
                                  cSenseSettingsPtr senseSettings);
    I2cCtrlBootloaderPtr getBootloaderController(ControllerTypes ctrlType, qint8 muxChannel = -1) override;
    I2cCtrlCommonInfoPtrUnique getCommonInfoController(ControllerTypes ctrlType, qint8 muxChannel = -1) override;

private:
    I2cSettingsPtr m_i2cSettings;
    cSenseSettingsPtr m_senseSettings;
};

typedef std::shared_ptr<TestHotPlugCtrlFactoryI2cCtrl> TestHotPlugCtrlFactoryI2cCtrlPtr;

#endif // TESTHOTPLUGCTRLFACTORYI2CCTRL_H
