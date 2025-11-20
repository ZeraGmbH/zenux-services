#ifndef TESTHOTPLUGCTRLFACTORYI2CCTRL_H
#define TESTHOTPLUGCTRLFACTORYI2CCTRL_H

#include "sensesettings.h"
#include "testfactoryi2cctrl.h"
#include "i2csettings.h"

class TestHotPlugCtrlFactoryI2cCtrl : public TestFactoryI2cCtrl
{
public:
    TestHotPlugCtrlFactoryI2cCtrl(std::shared_ptr<I2cSettings> i2cSettings,
                                  std::shared_ptr<cSenseSettings> senseSettings);
    I2cCtrlBootloaderPtr getBootloaderController(ControllerTypes ctrlType, quint8 muxChannel = -1) override;
    I2cCtrlCommonInfoPtrUnique getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel = -1) override;

private:
    std::shared_ptr<I2cSettings> m_i2cSettings;
    std::shared_ptr<cSenseSettings> m_senseSettings;
};

typedef std::shared_ptr<TestHotPlugCtrlFactoryI2cCtrl> TestHotPlugCtrlFactoryI2cCtrlPtr;

#endif // TESTHOTPLUGCTRLFACTORYI2CCTRL_H
