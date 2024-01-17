#include "factoryi2cctrl.h"
#include "atmelwatcher.h"
#include "i2cctrlcriticalstatus.h"

constexpr int defaultDebugLevel = 1;

FactoryI2cCtrl::FactoryI2cCtrl(cI2CSettings *i2cSettings) :
    m_i2cSettings(i2cSettings)
{
}

I2cCtrlCriticalStatusPtr FactoryI2cCtrl::getCriticalStatusController()
{
    return std::make_unique<I2cCtrlCriticalStatus>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::relaisCtrlI2cAddress), defaultDebugLevel);
}

AtmelWatcherInterfacePtr FactoryI2cCtrl::createAtmelWatcher(QString devnode)
{
    return std::make_unique<cAtmelWatcher>(devnode);
}

I2cCtrlEepromPermissionPtr FactoryI2cCtrl::getPermissionCheckController()
{
    return getRelaisController();
}

I2cCtrlCommonVersionsPtrUnique FactoryI2cCtrl::getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel)
{
    switch(ctrlType) {
    case CTRL_TYPE_RELAIS:
        return getRelaisController();

    case CTRL_TYPE_SYSTEM:
        return getSystemController();
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
    return getRelaisController();
}

I2cCtrlAccumulatorPtr FactoryI2cCtrl::getAccumulatorController()
{
    return std::make_unique<AtmelCtrlSystem>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::sysCtrlI2cAddress), defaultDebugLevel);
}

I2cCtrlRangesPtr FactoryI2cCtrl::getRangesController()
{
    return getRelaisController();
}

I2cCtrlMModePtr FactoryI2cCtrl::getMModeController()
{
    return getRelaisController();
}

I2cCtrlPllPtr FactoryI2cCtrl::getPllController()
{
    return getRelaisController();
}

std::unique_ptr<AtmelCtrlRelais> FactoryI2cCtrl::getRelaisController()
{
    return std::make_unique<AtmelCtrlRelais>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::relaisCtrlI2cAddress), defaultDebugLevel);
}

std::unique_ptr<AtmelCtrlSystem> FactoryI2cCtrl::getSystemController()
{
    return std::make_unique<AtmelCtrlSystem>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::sysCtrlI2cAddress), defaultDebugLevel);
}
