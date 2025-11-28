#include "testhotplugctrlfactoryi2cctrl.h"
#include "mocki2cctrlbootloader.h"
#include "testhotplugi2cctrlcommoninfo.h"

TestHotPlugCtrlFactoryI2cCtrl::TestHotPlugCtrlFactoryI2cCtrl(I2cSettingsPtr i2cSettings,
                                                             cSenseSettingsPtr senseSettings) :
    TestFactoryI2cCtrl(true),
    m_i2cSettings(i2cSettings),
    m_senseSettings(senseSettings)
{
}

I2cCtrlBootloaderPtr TestHotPlugCtrlFactoryI2cCtrl::getBootloaderController(ControllerTypes ctrlType, qint8 muxChannel)
{
    Q_UNUSED(ctrlType)
    return std::make_unique<MockI2cCtrlBootloader>(muxChannel, m_controllerRunState == BOOTLOADER_RUNNING);
}

I2cCtrlCommonInfoPtrUnique TestHotPlugCtrlFactoryI2cCtrl::getCommonInfoController(ControllerTypes ctrlType, qint8 muxChannel)
{
    Q_UNUSED(ctrlType)
    Q_UNUSED(muxChannel)
    return std::make_unique<TestHotplugI2cCtrlCommonInfo>(m_i2cSettings.get(), muxChannel);
}
