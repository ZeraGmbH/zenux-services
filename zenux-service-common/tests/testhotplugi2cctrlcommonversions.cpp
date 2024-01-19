#include "testhotplugi2cctrlcommonversions.h"

TestHotplugI2cCtrlCommonVersions::TestHotplugI2cCtrlCommonVersions(I2cSettings *i2cSettings, quint8 muxChannel, bool responding, int &instanceCount) :
    m_i2cSettings(i2cSettings),
    m_muxChannel(muxChannel),
    m_responding(responding),
    m_instanceCount(instanceCount)
{
    m_instanceCount++;
}

TestHotplugI2cCtrlCommonVersions::~TestHotplugI2cCtrlCommonVersions()
{
    m_instanceCount--;
}

ZeraMControllerIoTemplate::atmelRM TestHotplugI2cCtrlCommonVersions::readCTRLVersion(QString &answer)
{
    if(m_responding)
        answer = "EMOB test";
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestHotplugI2cCtrlCommonVersions::readPCBVersion(QString &)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

QString TestHotplugI2cCtrlCommonVersions::getDevnode()
{
    return m_i2cSettings->getDeviceNode();
}

quint8 TestHotplugI2cCtrlCommonVersions::getAdrCtrl()
{
    return m_i2cSettings->getI2CAdress(i2cSettings::emobCtrlI2cAddress);
}

quint8 TestHotplugI2cCtrlCommonVersions::getAdrMux()
{
    return m_i2cSettings->getI2CAdress(i2cSettings::muxerI2cAddress);
}

quint8 TestHotplugI2cCtrlCommonVersions::getMuxChannel()
{
    return m_muxChannel;
}
