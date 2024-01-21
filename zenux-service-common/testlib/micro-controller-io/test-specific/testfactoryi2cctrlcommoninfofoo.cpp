#include "testfactoryi2cctrlcommoninfofoo.h"
#include "mockcontrollertypename.h"
#include "mocki2cctrlcommoninfo.h"
#include "mocki2cctrlranges.h"
#include "mocki2cctrleeprompermission.h"
#include "testi2cctrldeviceident.h"
#include "mocki2cctrlmmode.h"

TestFactoryI2cCtrlCommonInfoFoo::TPersitentControllerData TestFactoryI2cCtrlCommonInfoFoo::m_persitentData;

I2cCtrlEepromPermissionPtr TestFactoryI2cCtrlCommonInfoFoo::getPermissionCheckController()
{
    return std::make_unique<MockI2cCtrlEepromPermission>(m_permission);
}

I2cCtrlCommonInfoPtrUnique TestFactoryI2cCtrlCommonInfoFoo::getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel)
{
    Q_UNUSED(muxChannel)
    return std::make_unique<MockI2cCtrlCommonInfo>("foo" + MockControllerTypeName::getCtrlTypeName(ctrlType));
}

I2cCtrlDeviceIdentPtr TestFactoryI2cCtrlCommonInfoFoo::getDeviceIdentController()
{
    return std::make_unique<TestI2cCtrlDeviceIdent>(m_persitentData.m_serialNumber,
                                                    m_persitentData.m_writablePcbVersion);
}

I2cCtrlRangesPtr TestFactoryI2cCtrlCommonInfoFoo::getRangesController()
{
    return std::make_unique<MockI2cCtrlRanges>();
}

I2cCtrlMModePtr TestFactoryI2cCtrlCommonInfoFoo::getMModeController()
{
    return std::make_unique<MockI2cCtrlMMode>();
}
