#include "hotplugctrltest.h"

HotPlugCtrlTest::HotPlugCtrlTest(I2cSettings *i2cSettings, quint8 muxChannel, bool responding, int &instanceCount) :
    m_i2cSettings(i2cSettings),
    m_muxChannel(muxChannel),
    m_responding(responding),
    m_instanceCount(instanceCount)
{
    m_instanceCount++;
}

HotPlugCtrlTest::~HotPlugCtrlTest()
{
    m_instanceCount--;
}

ZeraMControllerIoTemplate::atmelRM HotPlugCtrlTest::readCTRLVersion(QString &answer)
{
    if(m_responding)
        answer = "EMOB test";
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM HotPlugCtrlTest::readPCBVersion(QString &)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

QString HotPlugCtrlTest::getDevnode()
{
    return m_i2cSettings->getDeviceNode();
}

quint8 HotPlugCtrlTest::getAdrCtrl()
{
    return m_i2cSettings->getI2CAdress(i2cSettings::emobCtrlI2cAddress);
}

quint8 HotPlugCtrlTest::getAdrMux()
{
    return m_i2cSettings->getI2CAdress(i2cSettings::muxerI2cAddress);
}

quint8 HotPlugCtrlTest::getMuxChannel()
{
    return m_muxChannel;
}
