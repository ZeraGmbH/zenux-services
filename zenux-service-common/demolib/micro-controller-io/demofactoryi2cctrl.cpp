#include "demofactoryi2cctrl.h"
#include "controllertypename.h"
#include "mocki2cctrlemob.h"
#include "mocki2cctrlcommoninfo.h"
#include "mockctrlheartbeatwait.h"
#include "mocki2cctrlbootloader.h"
#include "mocki2cctrlcommoninfo.h"
#include "mocki2cctrlcriticalstatus.h"
#include "demoi2cctrleeprompermission.h"
#include "demoi2cctrlaccu.h"
#include "demoi2cctrldeviceident.h"
#include "demoi2cctrlranges.h"
#include "mocki2cctrlmmode.h"
#include "demoi2cctrlpll.h"
#include "mocki2cctrlclampstatus.h"
#include "demoi2cctrlcputemperature.h"

DemoFactoryI2cCtrl::TPersitentControllerData DemoFactoryI2cCtrl::m_persitentData;

DemoFactoryI2cCtrl::DemoFactoryI2cCtrl(SettingsContainerPtr settings) :
    m_settings(std::move(settings))
{
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
    return std::make_unique<DemoI2cCtrlEepromPermission>();
}

I2cCtrlCommonInfoPtrUnique DemoFactoryI2cCtrl::getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel)
{
    Q_UNUSED(muxChannel)
    return std::make_unique<MockI2cCtrlCommonInfo>(ControllerTypeName::getCtrlTypeName(ctrlType));
}

I2cCtrlDeviceIdentPtr DemoFactoryI2cCtrl::getDeviceIdentController()
{
    return std::make_unique<DemoI2cCtrlDeviceIdent>(m_persitentData.m_serialNumber,
                                                    m_persitentData.m_FPGAVersion,
                                                    m_persitentData.m_writablePcbVersion);
}

I2cCtrlAccumulatorPtr DemoFactoryI2cCtrl::getAccuController()
{
    return std::make_unique<DemoI2cCtrlAccu>();
}

I2cCtrlRangesPtr DemoFactoryI2cCtrl::getRangesController()
{
    return std::make_unique<DemoI2cCtrlRanges>();
}

I2cCtrlMModePtr DemoFactoryI2cCtrl::getMModeController()
{
    return std::make_unique<MockI2cCtrlMMode>();
}

I2cCtrlPllPtr DemoFactoryI2cCtrl::getPllController()
{
    return std::make_unique<DemoI2cCtrlPll>(m_settings->getSamplingSettings(), m_persitentData.m_pllChannel);
}

I2cCtrlClampStatusPtr DemoFactoryI2cCtrl::getClampStatusController()
{
    return std::make_unique<MockI2cCtrlClampStatus>();
}

I2cCtrlCpuTemperaturePtr DemoFactoryI2cCtrl::getCpuTemperatureController()
{
    return std::make_unique<DemoI2cCtrlCpuTemperature>();
}

I2cCtrlEMOBPtr DemoFactoryI2cCtrl::getEmobController(quint8 muxChannel)
{
    Q_UNUSED(muxChannel)
    return std::make_unique<MockI2cCtrlEMOB>();
}

I2cCtrlBootloaderPtr DemoFactoryI2cCtrl::getBootloaderController()
{
    return std::make_unique<MockI2cCtrlBootloader>();
}

