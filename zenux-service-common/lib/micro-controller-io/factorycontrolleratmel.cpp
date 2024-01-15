#include "factorycontrolleratmel.h"
#include "atmelctrlrelais.h"
#include "atmelctrlsystem.h"
#include "atmelwatcher.h"

constexpr int defaultDebugLevel = 1;

FactoryControllerAtmel::FactoryControllerAtmel(cI2CSettings *i2cSettings) :
    m_i2cSettings(i2cSettings)
{
}

AtmelWatcherInterfacePtr FactoryControllerAtmel::createAtmelWatcher(QString devnode)
{
    return std::make_unique<cAtmelWatcher>(devnode);
}

AtmelPermissionTemplatePtrU FactoryControllerAtmel::getPermissionCheckController()
{
    return std::make_unique<AtmelCtrlRelais>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::relaisCtrlI2cAddress), defaultDebugLevel);
}

AtmelCommonVersionsPtrU FactoryControllerAtmel::getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel)
{
    switch(ctrlType) {
    case CTRL_TYPE_RELAIS:
        return std::make_unique<AtmelCtrlRelais>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::relaisCtrlI2cAddress), defaultDebugLevel);

    case CTRL_TYPE_SYSTEM:
        return std::make_unique<AtmelCtrlSystem>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::sysCtrlI2cAddress), defaultDebugLevel);
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
        qFatal("Controller type %i does not support AtmelCommonVersions", ctrlType);
        return nullptr;
    }
}

AtmelDeviceIdentificationDataU FactoryControllerAtmel::getDeviceIdentificationController()
{
    return std::make_unique<AtmelCtrlRelais>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::relaisCtrlI2cAddress), defaultDebugLevel);
}

AtmelAccumulatorHandlerPtrU FactoryControllerAtmel::getAccumulatorController()
{
    return std::make_unique<AtmelCtrlSystem>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::sysCtrlI2cAddress), defaultDebugLevel);
}

AtmelRangesPtrU FactoryControllerAtmel::getRangesController()
{
    return std::make_unique<AtmelCtrlRelais>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::relaisCtrlI2cAddress), defaultDebugLevel);
}

AtmelMModesPtrU FactoryControllerAtmel::getMModeController()
{
    return std::make_unique<AtmelCtrlRelais>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::relaisCtrlI2cAddress), defaultDebugLevel);
}
