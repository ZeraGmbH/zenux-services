#include "testfactoryi2cctrlcommoninfofoo.h"
#include "mockcontrollertypename.h"
#include "mockctrlheartbeatwait.h"
#include "mocki2cctrlbootloader.h"
#include "testi2cctrlcommoninfofoo.h"
#include "mocki2cctrlcriticalstatus.h"
#include "mocki2cctrleeprompermission.h"
#include "mocki2cctrlaccumulator.h"
#include "mocki2cctrldeviceident.h"
#include "mocki2cctrlranges.h"
#include "mocki2cctrlmmode.h"
#include "mocki2cctrlpll.h"
#include "mocki2cctrlclampstatus.h"

TestFactoryI2cCtrlCommonInfoFoo::TPersitentControllerData TestFactoryI2cCtrlCommonInfoFoo::m_persitentData;

TestFactoryI2cCtrlCommonInfoFoo::TestFactoryI2cCtrlCommonInfoFoo(bool initialPermission)
{
    m_persitentData.m_permission = initialPermission;
}

AbstractCtrlHeartbeatWaitPtr TestFactoryI2cCtrlCommonInfoFoo::createCtrlHeartbeatWait(QString devnode)
{
    Q_UNUSED(devnode)
    return std::make_unique<MockCtrlHeartbeatWait>();
}

I2cCtrlCriticalStatusPtr TestFactoryI2cCtrlCommonInfoFoo::getCriticalStatusController()
{
    return std::make_unique<MockI2cCtrlCriticalStatus>(m_persitentData.m_criticalStatus,
                                                     m_persitentData.m_criticalStatusMask);
}

I2cCtrlEepromPermissionPtr TestFactoryI2cCtrlCommonInfoFoo::getPermissionCheckController()
{
    return std::make_unique<MockI2cCtrlEepromPermission>(m_persitentData.m_permission);
}

I2cCtrlCommonInfoPtrUnique TestFactoryI2cCtrlCommonInfoFoo::getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel)
{
    Q_UNUSED(muxChannel)
    return std::make_unique<TestI2cCtrlCommonInfoFoo>(MockControllerTypeName::getCtrlTypeName(ctrlType));
}

I2cCtrlDeviceIdentPtr TestFactoryI2cCtrlCommonInfoFoo::getDeviceIdentController()
{
    return std::make_unique<MockI2cCtrlDeviceIdent>(m_persitentData.m_deviceName,
                                                               m_persitentData.m_serialNumber,
                                                               m_persitentData.m_FPGAVersion,
                                                               m_persitentData.m_writablePcbVersion);
}

I2cCtrlAccumulatorPtr TestFactoryI2cCtrlCommonInfoFoo::getAccuController()
{
    return std::make_unique<MockI2cCtrlAccumulator>();
}

I2cCtrlRangesPtr TestFactoryI2cCtrlCommonInfoFoo::getRangesController()
{
    return std::make_unique<MockI2cCtrlRanges>();
}

I2cCtrlMModePtr TestFactoryI2cCtrlCommonInfoFoo::getMModeController()
{
    return std::make_unique<MockI2cCtrlMMode>();
}

I2cCtrlPllPtr TestFactoryI2cCtrlCommonInfoFoo::getPllController()
{
    return std::make_unique<MockI2cCtrlPll>(m_persitentData.m_pllChannel);
}

I2cCtrlClampStatusPtr TestFactoryI2cCtrlCommonInfoFoo::getClampStatusController()
{
    return std::make_unique<MockI2cCtrlClampStatus>();
}

I2cCtrlBootloaderPtr TestFactoryI2cCtrlCommonInfoFoo::getBootloaderController()
{
    return std::make_unique<MockI2cCtrlBootloader>();
}
