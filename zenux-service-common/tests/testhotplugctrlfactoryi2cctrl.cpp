#include "testhotplugctrlfactoryi2cctrl.h"
#include "testhotplugi2cctrlcommoninfo.h"

TestHotPlugCtrlFactoryI2cCtrl::TestHotPlugCtrlFactoryI2cCtrl(I2cSettings *i2cSettings) :
    TestFactoryI2cCtrl(true),
    m_i2cSettings(i2cSettings)
{
}

I2cCtrlCommonInfoPtrUnique TestHotPlugCtrlFactoryI2cCtrl::getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel)
{
    Q_UNUSED(ctrlType)
    Q_UNUSED(muxChannel)
    return std::make_unique<TestHotplugI2cCtrlCommonInfo>(m_i2cSettings, muxChannel);
}
