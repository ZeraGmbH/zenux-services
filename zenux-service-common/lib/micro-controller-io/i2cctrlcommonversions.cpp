#include "i2cctrlcommonversions.h"

I2cCtrlCommonVersions::I2cCtrlCommonVersions(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwGetCtrlVersion = 0x0003,
    hwGetPCBVersion = 0x0005,
    hwGetStatus = 0x1103
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlCommonVersions::readPCBVersion(QString &answer)
{
    return m_ctrlIo.readVariableLenText(hwGetPCBVersion, answer);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlCommonVersions::readCTRLVersion(QString &answer)
{
    return m_ctrlIo.readVariableLenText(hwGetCtrlVersion, answer);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlCommonVersions::readChannelStatus(quint8 channel, quint8 &stat)
{
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIo::cmdexecfault;
    quint8 answ[2];
    hw_cmd CMD (hwGetStatus, channel, nullptr, 0);
    m_ctrlIo.writeCommand(&CMD, answ, 2);
    if(m_ctrlIo.getLastErrorMask() == 0) {
        stat = answ[0];
        ret = ZeraMControllerIo::cmddone;
    }
    return ret;
}
