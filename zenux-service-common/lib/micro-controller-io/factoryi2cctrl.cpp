#include "factoryi2cctrl.h"
#include "atmelwatcher.h"
#include "i2cctrlbootloader.h"
#include "i2cctrlclampstatus.h"
#include "i2cctrlcommonversions.h"
#include "i2cctrlcriticalstatus.h"
#include "i2cctrldeviceidentificationdata.h"
#include "i2cctrleeprompermission.h"
#include "i2cctrlmmode.h"
#include "i2cctrlpll.h"
#include "i2cctrlranges.h"

constexpr int defaultDebugLevel = 1;

FactoryI2cCtrl::FactoryI2cCtrl(cI2CSettings *i2cSettings) :
    m_i2cSettings(i2cSettings)
{
}

I2cCtrlCriticalStatusPtr FactoryI2cCtrl::getCriticalStatusController()
{
    return std::make_unique<I2cCtrlCriticalStatus>(m_i2cSettings->getDeviceNode(), getRelaisCtrlI2cAddress(), defaultDebugLevel);
}

AtmelWatcherInterfacePtr FactoryI2cCtrl::createAtmelWatcher(QString devnode)
{
    return std::make_unique<cAtmelWatcher>(devnode);
}

I2cCtrlEepromPermissionPtr FactoryI2cCtrl::getPermissionCheckController()
{
    return std::make_unique<I2cCtrlEepromPermission>(m_i2cSettings->getDeviceNode(), getRelaisCtrlI2cAddress(), defaultDebugLevel);
}

I2cCtrlCommonVersionsPtrUnique FactoryI2cCtrl::getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel)
{
    switch(ctrlType) {
    case CTRL_TYPE_RELAIS:
        return std::make_unique<I2cCtrlCommonVersions>(m_i2cSettings->getDeviceNode(), getRelaisCtrlI2cAddress(), defaultDebugLevel);

    case CTRL_TYPE_SYSTEM:
        return std::make_unique<I2cCtrlCommonVersions>(m_i2cSettings->getDeviceNode(), getSystemCtrlI2cAddress(), defaultDebugLevel);
        break;

    /* not tested yet
    case CTRL_TYPE_EMOB: {
        ZeraMcontrollerIoPtr i2cCtrl = std::make_shared<ZeraMControllerIo>(m_i2cSettings->getDeviceNode(),
                                                                           m_i2cSettings->getI2CAdress(i2cSettings::emobCtrlI2cAddress),
                                                                           defaultDebugLevel);
        return std::make_unique<AtmelCtrlEmob>(i2cCtrl,
                                               m_i2cSettings->getDeviceNode(),
                                               m_i2cSettings->getI2CAdress(i2cSettings::muxerI2cAddress),
                                               muxChannel);
    }*/
    default:
        qFatal("Controller type %i does not support AbstractI2cCtrlCommonVersions", ctrlType);
        return nullptr;
    }
}

I2cCtrlDeviceIdentificationDataPtr FactoryI2cCtrl::getDeviceIdentificationController()
{
    return std::make_unique<I2cCtrlDeviceIdentificationData>(m_i2cSettings->getDeviceNode(), getRelaisCtrlI2cAddress(), defaultDebugLevel);
}

I2cCtrlAccumulatorPtr FactoryI2cCtrl::getAccumulatorController()
{
    return std::make_unique<AtmelCtrlSystem>(m_i2cSettings->getDeviceNode(), getSystemCtrlI2cAddress(), defaultDebugLevel);
}

I2cCtrlRangesPtr FactoryI2cCtrl::getRangesController()
{
    return std::make_unique<I2cCtrlRanges>(m_i2cSettings->getDeviceNode(), getRelaisCtrlI2cAddress(), defaultDebugLevel);
}

I2cCtrlMModePtr FactoryI2cCtrl::getMModeController()
{
    return std::make_unique<I2cCtrlMMode>(m_i2cSettings->getDeviceNode(), getRelaisCtrlI2cAddress(), defaultDebugLevel);
}

I2cCtrlPllPtr FactoryI2cCtrl::getPllController()
{
    return std::make_unique<I2cCtrlPll>(m_i2cSettings->getDeviceNode(), getRelaisCtrlI2cAddress(), defaultDebugLevel);
}

I2cCtrlClampStatusPtr FactoryI2cCtrl::getClampStatusController()
{
    return std::make_unique<I2cCtrlClampStatus>(m_i2cSettings->getDeviceNode(), getRelaisCtrlI2cAddress(), defaultDebugLevel);
}

I2cCtrlBootloaderPtr FactoryI2cCtrl::getBootloaderController()
{
    return std::make_unique<I2cCtrlBootloader>(m_i2cSettings->getDeviceNode(), getRelaisCtrlI2cAddress(), defaultDebugLevel);
}

quint8 FactoryI2cCtrl::getRelaisCtrlI2cAddress()
{
    return m_i2cSettings->getI2CAdress(i2cSettings::relaisCtrlI2cAddress);
}

quint8 FactoryI2cCtrl::getSystemCtrlI2cAddress()
{
    return m_i2cSettings->getI2CAdress(i2cSettings::sysCtrlI2cAddress);
}
