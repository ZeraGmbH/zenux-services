#include "testhotplugi2cctrlcommoninfo.h"

TestHotplugI2cCtrlCommonInfo::TestHotplugI2cCtrlCommonInfo(I2cSettings *i2cSettings, quint8 muxChannel) :
    m_i2cSettings(i2cSettings),
    m_muxChannel(muxChannel)
{
}

ZeraMControllerIoTemplate::atmelRM TestHotplugI2cCtrlCommonInfo::readCTRLVersion(QString &answer)
{
    answer = "EMOB test";
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestHotplugI2cCtrlCommonInfo::readPCBInfo(QString &)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

QString TestHotplugI2cCtrlCommonInfo::getDevnode()
{
    return m_i2cSettings->getDeviceNode();
}

quint8 TestHotplugI2cCtrlCommonInfo::getAdrCtrl()
{
    return m_i2cSettings->getI2CAdress(i2cSettings::emobCtrlI2cAddress);
}

quint8 TestHotplugI2cCtrlCommonInfo::getAdrMux()
{
    return m_i2cSettings->getI2CAdress(i2cSettings::muxerI2cAddress);
}

quint8 TestHotplugI2cCtrlCommonInfo::getMuxChannel()
{
    return m_muxChannel;
}
