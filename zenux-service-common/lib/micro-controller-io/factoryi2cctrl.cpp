#include "factoryi2cctrl.h"
#include "ctrlheartbeatwait.h"
#include "i2cctrlaccu.h"
#include "i2cctrlbootloader.h"
#include "i2cctrlclampstatus.h"
#include "i2cctrlcommoninfo.h"
#include "i2cctrlcommoninfoaccu.h"
#include "i2cctrlcommoninfoemob.h"
#include "i2cctrlcriticalstatus.h"
#include "i2cctrldeviceidentificationdata.h"
#include "i2cctrleeprompermission.h"
#include "i2cctrlmmode.h"
#include "i2cctrlpll.h"
#include "i2cctrlranges.h"
#include "i2cctrlcputemperature.h"
#include "i2cctrlreaderror.h"

FactoryI2cCtrl::FactoryI2cCtrl(I2cSettings *i2cSettings) :
    m_i2cSettings(i2cSettings),
    m_deviceNode(i2cSettings->getDeviceNode()),
    m_debugLevel(i2cSettings->getDebugLevel())
{
}

I2cCtrlCriticalStatusPtr FactoryI2cCtrl::getCriticalStatusController()
{
    return std::make_unique<I2cCtrlCriticalStatus>(m_deviceNode, getRelaisCtrlI2cAddress(), m_debugLevel);
}

AbstractCtrlHeartbeatWaitPtr FactoryI2cCtrl::createCtrlHeartbeatWait(QString devnode)
{
    return std::make_unique<CtrlHeartbeatWait>(devnode);
}

I2cCtrlEepromPermissionPtr FactoryI2cCtrl::getPermissionCheckController()
{
    return std::make_unique<I2cCtrlEepromPermission>(m_deviceNode, getRelaisCtrlI2cAddress(), m_debugLevel);
}

I2cCtrlCommonInfoPtrUnique FactoryI2cCtrl::getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel)
{
    switch(ctrlType) {
    case CTRL_TYPE_RELAIS:
        return std::make_unique<I2cCtrlCommonInfo>(m_deviceNode, getRelaisCtrlI2cAddress(), m_debugLevel);

    case CTRL_TYPE_SYSTEM:
        return std::make_unique<I2cCtrlCommonInfo>(m_deviceNode, getSystemCtrlI2cAddress(), m_debugLevel);
        break;

    case CTRL_TYPE_EMOB:
        return std::make_unique<I2cCtrlCommonInfoEmob>(m_deviceNode, getEmobCtrlI2cAddress(),
                                                           getEmobMuxI2cAddress(), muxChannel,
                                                           0); // i2c error can occure if clamp is connected
        break;

    case CTRL_TYPE_ACCU:
        return std::make_unique<I2cCtrlCommonInfoAccu>(m_deviceNode, getSystemCtrlI2cAddress(), m_debugLevel);
        break;

    default:
        qFatal("Controller type %i does not support AbstractI2cCtrlCommonInfo", ctrlType);
        return nullptr;
    }
}

I2cCtrlDeviceIdentPtr FactoryI2cCtrl::getDeviceIdentController()
{
    return std::make_unique<I2cCtrlDeviceIdent>(m_deviceNode, getRelaisCtrlI2cAddress(), m_debugLevel);
}

I2cCtrlAccumulatorPtr FactoryI2cCtrl::getAccuController()
{
    return std::make_unique<I2cCtrlAccu>(m_deviceNode, getSystemCtrlI2cAddress(), m_debugLevel);
}

I2cCtrlRangesPtr FactoryI2cCtrl::getRangesController()
{
    return std::make_unique<I2cCtrlRanges>(m_deviceNode, getRelaisCtrlI2cAddress(), m_debugLevel);
}

I2cCtrlReadErrorPtr FactoryI2cCtrl::getErrorlogController()
{
     return std::make_unique<I2cCtrlReadError>(m_deviceNode, getSystemCtrlI2cAddress(), getRelaisCtrlI2cAddress(), getEmobCtrlI2cAddress(), m_debugLevel);
}

I2cCtrlMModePtr FactoryI2cCtrl::getMModeController()
{
    return std::make_unique<I2cCtrlMMode>(m_deviceNode, getRelaisCtrlI2cAddress(), m_debugLevel);
}

I2cCtrlPllPtr FactoryI2cCtrl::getPllController()
{
    return std::make_unique<I2cCtrlPll>(m_deviceNode, getRelaisCtrlI2cAddress(), m_debugLevel);
}

I2cCtrlClampStatusPtr FactoryI2cCtrl::getClampStatusController()
{
    return std::make_unique<I2cCtrlClampStatus>(m_deviceNode, getRelaisCtrlI2cAddress(), m_debugLevel);
}

I2cCtrlBootloaderPtr FactoryI2cCtrl::getBootloaderController()
{
    return std::make_unique<I2cCtrlBootloader>(m_deviceNode, getRelaisCtrlI2cAddress(), m_debugLevel);
}

quint8 FactoryI2cCtrl::getRelaisCtrlI2cAddress()
{
    return m_i2cSettings->getI2CAdress(i2cSettings::relaisCtrlI2cAddress);
}

quint8 FactoryI2cCtrl::getSystemCtrlI2cAddress()
{
    return m_i2cSettings->getI2CAdress(i2cSettings::sysCtrlI2cAddress);
}

quint8 FactoryI2cCtrl::getEmobCtrlI2cAddress()
{
    return m_i2cSettings->getI2CAdress(i2cSettings::emobCtrlI2cAddress);
}

quint8 FactoryI2cCtrl::getEmobMuxI2cAddress()
{
    return m_i2cSettings->getI2CAdress(i2cSettings::muxerI2cAddress);
}


I2cCtrlCpuTemperaturePtr FactoryI2cCtrl::getCpuTemperatureController()          // ist das richtig???
{
    return std::make_unique<I2cCtrlCpuTemperature>(m_deviceNode, getSystemCtrlI2cAddress(), m_debugLevel);
}
