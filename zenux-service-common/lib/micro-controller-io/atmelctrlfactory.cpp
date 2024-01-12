#include "atmelctrlfactory.h"
#include "atmelctrlrelais.h"
#include "atmelctrlsystem.h"
//#include "atmelctrlemob.h"

constexpr int defaultDebugLevel = 1;

AtmelCtrlFactory::AtmelCtrlFactory(cI2CSettings *i2cSettings) :
    m_i2cSettings(i2cSettings)
{
}

AtmelPermissionTemplatePtrU AtmelCtrlFactory::getPermissionCheckController()
{
    return std::make_unique<AtmelCtrlRelais>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::relaisCtrlI2cAddress), defaultDebugLevel);
}

AtmelCommonVersionsPtrU AtmelCtrlFactory::getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel)
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

AtmelAccumulatorHandlerPtrU AtmelCtrlFactory::getAccumulatorController()
{
    return std::make_unique<AtmelCtrlSystem>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::sysCtrlI2cAddress), defaultDebugLevel);
}
