#include "testfactoryi2cctrl.h"
#include "mockctrlheartbeatwait.h"
#include "mocki2cctrlbootloader.h"
#include "mocki2cctrlemob.h"
#include "testi2cctrlaccu.h"
#include "testi2cctrlcommoninfo.h"
#include "mocki2cctrlcriticalstatus.h"
#include "testi2cctrlranges.h"
#include "mocki2cctrlmmode.h"
#include "testi2cctrlpll.h"
#include "mocki2cctrlclampstatus.h"
#include "testi2cctrldeviceident.h"
#include "testi2cctrleeprompermission.h"
#include "testi2cctrlcputemperature.h"

TestFactoryI2cCtrl::TPersitentControllerData TestFactoryI2cCtrl::m_persitentData;

TestFactoryI2cCtrl::TestFactoryI2cCtrl(bool initialPermission)
{
    m_persitentData.m_permission = initialPermission;
}

void TestFactoryI2cCtrl::setRangeGetSetDelay(int rangeGetSetDelay)
{
    m_rangeGetSetDelay = rangeGetSetDelay;
}

AbstractCtrlHeartbeatWaitPtr TestFactoryI2cCtrl::createCtrlHeartbeatWait(QString devnode)
{
    Q_UNUSED(devnode)
    return std::make_unique<MockCtrlHeartbeatWait>();
}

I2cCtrlCriticalStatusPtr TestFactoryI2cCtrl::getCriticalStatusController()
{
    return std::make_unique<MockI2cCtrlCriticalStatus>(m_persitentData.m_criticalStatus,
                                                       m_persitentData.m_criticalStatusMask);
}

I2cCtrlEepromPermissionPtr TestFactoryI2cCtrl::getPermissionCheckController()
{
    return std::make_unique<TestI2cCtrlEepromPermission>(m_persitentData.m_permission);
}

I2cCtrlCommonInfoPtrUnique TestFactoryI2cCtrl::getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel)
{
    Q_UNUSED(ctrlType)
    Q_UNUSED(muxChannel)
    return std::make_unique<TestI2cCtrlCommonInfo>();
}

I2cCtrlDeviceIdentPtr TestFactoryI2cCtrl::getDeviceIdentController()
{
    return std::make_unique<TestI2cCtrlDeviceIdent>(m_persitentData.m_serialNumber,
                                                    m_persitentData.m_writablePcbVersion);
}

I2cCtrlAccumulatorPtr TestFactoryI2cCtrl::getAccuController()
{
    return std::make_unique<TestI2cCtrlAccu>();
}

I2cCtrlRangesPtr TestFactoryI2cCtrl::getRangesController()
{
    return std::make_unique<TestI2cCtrlRanges>(m_rangeGetSetDelay);
}

I2cCtrlMModePtr TestFactoryI2cCtrl::getMModeController()
{
    return std::make_unique<MockI2cCtrlMMode>();
}

I2cCtrlPllPtr TestFactoryI2cCtrl::getPllController()
{
    return std::make_unique<TestI2cCtrlPll>(m_persitentData.m_pllChannel);
}

I2cCtrlClampStatusPtr TestFactoryI2cCtrl::getClampStatusController()
{
    return std::make_unique<MockI2cCtrlClampStatus>();
}

I2cCtrlCpuTemperaturePtr TestFactoryI2cCtrl::getCpuTemperatureController()
{
    return std::make_unique<TestI2cCtrlCpuTmperature>();
}

I2cCtrlEMOBPtr TestFactoryI2cCtrl::getEmobController(quint8 muxChannel)
{
    Q_UNUSED(muxChannel)
    return std::make_unique<MockI2cCtrlEMOB>();
}

I2cCtrlBootloaderPtr TestFactoryI2cCtrl::getBootloaderController()
{
    return std::make_unique<MockI2cCtrlBootloader>();
}

