#include "demofactoryi2cctrl.h"
#include "demoi2cctrlcommoninfo.h"
#include "mockctrlheartbeatwait.h"
#include "mocki2cctrlbootloader.h"
#include "demoi2cctrlcommoninfo.h"
#include "mocki2cctrlcriticalstatus.h"
#include "mocki2cctrleeprompermission.h"
#include "mocki2cctrlaccumulator.h"
#include "mocki2cctrldeviceidentificationdata.h"
#include "mocki2cctrlranges.h"
#include "mocki2cctrlmmode.h"
#include "mocki2cctrlpll.h"
#include "mocki2cctrlclampstatus.h"

DemoFactoryI2cCtrl::TPersitentControllerData DemoFactoryI2cCtrl::m_persitentData;

DemoFactoryI2cCtrl::DemoFactoryI2cCtrl(bool initialPermission)
{
    m_persitentData.m_permission = initialPermission;
}

AbstractCtrlHeartbeatWaitPtr DemoFactoryI2cCtrl::createCtrlHeartbeatWait(QString devnode)
{
    Q_UNUSED(devnode)
    return std::make_unique<MockCtrlHeartbeatWait>();
}

I2cCtrlCriticalStatusPtr DemoFactoryI2cCtrl::getCriticalStatusController()
{
    return std::make_unique<MockI2cCtrlCriticalStatus>(m_persitentData.m_criticalStatus,
                                                     m_persitentData.m_criticalStatusMask);
}

I2cCtrlEepromPermissionPtr DemoFactoryI2cCtrl::getPermissionCheckController()
{
    return std::make_unique<MockI2cCtrlEepromPermission>(m_persitentData.m_permission);
}

I2cCtrlCommonInfoPtrUnique DemoFactoryI2cCtrl::getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel)
{
    Q_UNUSED(muxChannel)
    return std::make_unique<DemoI2cCtrlCommonInfo>(getBaseName(ctrlType));
}

I2cCtrlDeviceIdentPtr DemoFactoryI2cCtrl::getDeviceIdentController()
{
    return std::make_unique<MockI2cCtrlDeviceIdent>(m_persitentData.m_deviceName,
                                                               m_persitentData.m_serialNumber,
                                                               m_persitentData.m_FPGAVersion,
                                                               m_persitentData.m_writablePcbVersion);
}

I2cCtrlAccumulatorPtr DemoFactoryI2cCtrl::getAccuController()
{
    return std::make_unique<MockI2cCtrlAccumulator>();
}

I2cCtrlRangesPtr DemoFactoryI2cCtrl::getRangesController()
{
    return std::make_unique<MockI2cCtrlRanges>();
}

I2cCtrlMModePtr DemoFactoryI2cCtrl::getMModeController()
{
    return std::make_unique<MockI2cCtrlMMode>();
}

I2cCtrlPllPtr DemoFactoryI2cCtrl::getPllController()
{
    return std::make_unique<MockI2cCtrlPll>(m_persitentData.m_pllChannel);
}

I2cCtrlClampStatusPtr DemoFactoryI2cCtrl::getClampStatusController()
{
    return std::make_unique<MockI2cCtrlClampStatus>();
}

I2cCtrlBootloaderPtr DemoFactoryI2cCtrl::getBootloaderController()
{
    return std::make_unique<MockI2cCtrlBootloader>();
}

QString DemoFactoryI2cCtrl::getBaseName(ControllerTypes ctrlType)
{
    switch(ctrlType) {
    default:
        return "Relais";
    case CTRL_TYPE_SYSTEM:
        return "System";
    case CTRL_TYPE_EMOB:
        return "Emob";
    }
}
