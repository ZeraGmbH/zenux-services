#include "testfactoryi2cctrlcriticalstatus.h"
#include "controllertypename.h"
#include "mocki2cctrlcommoninfo.h"
#include "mockctrlheartbeatwait.h"
#include "mocki2cctrlbootloader.h"
#include "mocki2cctrlcommoninfo.h"
#include "mocki2cctrlcriticalstatus.h"
#include "demoi2cctrleeprompermission.h"
#include "demoi2cctrlaccu.h"
#include "demoi2cctrldeviceident.h"
#include "mocki2cctrlranges.h"
#include "mocki2cctrlmmode.h"
#include "mocki2cctrlpll.h"
#include "mocki2cctrlclampstatus.h"

TestFactoryI2cCtrlCriticalStatus::TPersitentControllerData TestFactoryI2cCtrlCriticalStatus::m_persitentData;

TestFactoryI2cCtrlCriticalStatus::TestFactoryI2cCtrlCriticalStatus(quint16 initialCriticalStatus)
{
    m_persitentData.m_criticalStatus = initialCriticalStatus;
}

AbstractCtrlHeartbeatWaitPtr TestFactoryI2cCtrlCriticalStatus::createCtrlHeartbeatWait(QString devnode)
{
    Q_UNUSED(devnode)
    return std::make_unique<MockCtrlHeartbeatWait>();
}

I2cCtrlCriticalStatusPtr TestFactoryI2cCtrlCriticalStatus::getCriticalStatusController()
{
    return std::make_unique<MockI2cCtrlCriticalStatus>(m_persitentData.m_criticalStatus,
                                                       m_persitentData.m_criticalStatusMask);
}

I2cCtrlEepromPermissionPtr TestFactoryI2cCtrlCriticalStatus::getPermissionCheckController()
{
    return std::make_unique<DemoI2cCtrlEepromPermission>();
}

I2cCtrlCommonInfoPtrUnique TestFactoryI2cCtrlCriticalStatus::getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel)
{
    Q_UNUSED(muxChannel)
    return std::make_unique<MockI2cCtrlCommonInfo>(ControllerTypeName::getCtrlTypeName(ctrlType));
}

I2cCtrlDeviceIdentPtr TestFactoryI2cCtrlCriticalStatus::getDeviceIdentController()
{
    return std::make_unique<DemoI2cCtrlDeviceIdent>(m_persitentData.m_serialNumber,
                                                    m_persitentData.m_FPGAVersion,
                                                    m_persitentData.m_writablePcbVersion);
}

I2cCtrlAccumulatorPtr TestFactoryI2cCtrlCriticalStatus::getAccuController()
{
    return std::make_unique<DemoI2cCtrlAccu>();
}

I2cCtrlRangesPtr TestFactoryI2cCtrlCriticalStatus::getRangesController()
{
    return std::make_unique<MockI2cCtrlRanges>();
}

I2cCtrlMModePtr TestFactoryI2cCtrlCriticalStatus::getMModeController()
{
    return std::make_unique<MockI2cCtrlMMode>();
}

I2cCtrlPllPtr TestFactoryI2cCtrlCriticalStatus::getPllController()
{
    return std::make_unique<MockI2cCtrlPll>(m_persitentData.m_pllChannel);
}

I2cCtrlClampStatusPtr TestFactoryI2cCtrlCriticalStatus::getClampStatusController()
{
    return std::make_unique<MockI2cCtrlClampStatus>();
}

I2cCtrlBootloaderPtr TestFactoryI2cCtrlCriticalStatus::getBootloaderController()
{
    return std::make_unique<MockI2cCtrlBootloader>();
}
