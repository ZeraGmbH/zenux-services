#include "mockfactoryi2cctrl.h"
#include "mockatmelwatcher.h"
#include "mocki2cctrlcommonversions.h"
#include "mocki2cctrlcriticalstatus.h"
#include "mocki2cctrleeprompermission.h"
#include "mocki2cctrlaccumulator.h"
#include "mocki2cctrldeviceidentificationdata.h"
#include "mocki2cctrlranges.h"
#include "mocki2cctrlmmode.h"
#include "mocki2cctrlpll.h"
#include "mocki2cctrlclampstatus.h"

MockFactoryI2cCtrl::TPersitentControllerData MockFactoryI2cCtrl::m_persitentData;

MockFactoryI2cCtrl::MockFactoryI2cCtrl(bool initialPermission)
{
    m_persitentData.m_permission = initialPermission;
}

AtmelWatcherInterfacePtr MockFactoryI2cCtrl::createAtmelWatcher(QString devnode)
{
    Q_UNUSED(devnode)
    return std::make_unique<MockAtmelWatcher>();
}

I2cCtrlCriticalStatusPtr MockFactoryI2cCtrl::getCriticalStatusController()
{
    return std::make_unique<MockI2cCtrlCriticalStatus>(m_persitentData.m_criticalStatus,
                                                     m_persitentData.m_criticalStatusMask);
}

I2cCtrlEepromPermissionPtr MockFactoryI2cCtrl::getPermissionCheckController()
{
    return std::make_unique<MockI2cCtrlEepromPermission>(m_persitentData.m_permission);
}

I2cCtrlCommonVersionsPtrUnique MockFactoryI2cCtrl::getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel)
{
    Q_UNUSED(ctrlType)
    Q_UNUSED(muxChannel)
    return std::make_unique<MockI2cCtrlCommonVersions>(); // not prepared for updating pcbVersion write update...
}

I2cCtrlDeviceIdentificationDataPtr MockFactoryI2cCtrl::getDeviceIdentificationController()
{
    return std::make_unique<MockI2cCtrlDeviceIdentificationData>(m_persitentData.m_deviceName,
                                                               m_persitentData.m_serialNumber,
                                                               m_persitentData.m_LCAVersion,
                                                               m_persitentData.m_writablePcbVersion);
}

I2cCtrlAccumulatorPtr MockFactoryI2cCtrl::getAccumulatorController()
{
    return std::make_unique<MockI2cCtrlAccumulator>();
}

I2cCtrlRangesPtr MockFactoryI2cCtrl::getRangesController()
{
    return std::make_unique<MockI2cCtrlRanges>();
}

I2cCtrlMModePtr MockFactoryI2cCtrl::getMModeController()
{
    return std::make_unique<MockI2cCtrlMMode>();
}

I2cCtrlPllPtr MockFactoryI2cCtrl::getPllController()
{
    return std::make_unique<MockI2cCtrlPll>(m_persitentData.m_pllChannel);
}

I2cCtrlClampStatusPtr MockFactoryI2cCtrl::getClampStatusController()
{
    return std::make_unique<MockI2cCtrlClampStatus>();
}
