#include "factorycontrolleratmel.h"
#include "atmelwatcher.h"

constexpr int defaultDebugLevel = 1;

FactoryControllerAtmel::FactoryControllerAtmel(cI2CSettings *i2cSettings) :
    m_i2cSettings(i2cSettings)
{
}

AtmelCriticalStatusPtr FactoryControllerAtmel::getCriticalStatusController()
{
    return getRelaisController();
}

AtmelWatcherInterfacePtr FactoryControllerAtmel::createAtmelWatcher(QString devnode)
{
    return std::make_unique<cAtmelWatcher>(devnode);
}

AtmelPermissionTemplatePtrU FactoryControllerAtmel::getPermissionCheckController()
{
    return getRelaisController();
}

AtmelCommonVersionsPtrU FactoryControllerAtmel::getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel)
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

AtmelDeviceIdentificationDataU FactoryControllerAtmel::getDeviceIdentificationController()
{
    return getRelaisController();
}

AtmelAccumulatorHandlerPtrU FactoryControllerAtmel::getAccumulatorController()
{
    return std::make_unique<AtmelCtrlSystem>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::sysCtrlI2cAddress), defaultDebugLevel);
}

AtmelRangesPtrU FactoryControllerAtmel::getRangesController()
{
    return getRelaisController();
}

AtmelMModesPtrU FactoryControllerAtmel::getMModeController()
{
    return getRelaisController();
}

AtmelPllPtrU FactoryControllerAtmel::getPllController()
{
    return getRelaisController();
}

std::unique_ptr<AtmelCtrlRelais> FactoryControllerAtmel::getRelaisController()
{
    return std::make_unique<AtmelCtrlRelais>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::relaisCtrlI2cAddress), defaultDebugLevel);
}

std::unique_ptr<AtmelCtrlSystem> FactoryControllerAtmel::getSystemController()
{
    return std::make_unique<AtmelCtrlSystem>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::sysCtrlI2cAddress), defaultDebugLevel);
}
