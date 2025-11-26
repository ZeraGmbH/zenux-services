#include "i2cctrlemob.h"
#include <i2cmuxerscopedonoff.h>

I2cCtrlEMOB::I2cCtrlEMOB(QString deviceNodeName,
                         quint8 i2cAddress,
                         quint8 i2cAddressMux,
                         qint8 muxChannel,
                         quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel),
    m_i2cMuxer(I2cMultiplexerFactory::createPCA9547Muxer(deviceNodeName, i2cAddressMux, muxChannel))
{
}

enum hw_cmdcode
{
    readInstrumentSubType = 0x0010,
    hwSendPushbuttonPress = 0x0041,
    hwReadEmobLockState = 0x0060,      // old: hwReadEmobConnectionState
    hwGetErrorStatus    = 0x0062,
    hwClearErrorStatus = 0x0063
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlEMOB::readEmobInstrumentSubType(QString &answer)
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
    return m_ctrlIo.readVariableLenText(readInstrumentSubType, answer);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlEMOB::sendPushbuttonPress()
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
    hw_cmd CMD(hwSendPushbuttonPress, 0, nullptr, 0);
    m_ctrlIo.writeCommand(&CMD);
    if (m_ctrlIo.getLastErrorMask() != 0)
        return ZeraMControllerIo::cmdexecfault;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlEMOB::readEmobLockState(quint8 &status)
{
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIo::cmdexecfault;
    quint8 answ[2];
    I2cMuxerScopedOnOff i2cMuxerEnable(m_i2cMuxer);
    hw_cmd CMD(hwReadEmobLockState, 0, nullptr, 0);
    m_ctrlIo.writeCommand(&CMD, answ, 2);
    if (m_ctrlIo.getLastErrorMask() == 0) {
        status = answ[0];
        ret = ZeraMControllerIo::cmddone;
    }
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlEMOB::readEmobErrorStatus(quint8 &err)
{
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIo::cmdexecfault;
    quint8 answ[2];
    I2cMuxerScopedOnOff i2cMuxerEnable(m_i2cMuxer);
    hw_cmd CMD(hwGetErrorStatus, 0, nullptr, 0);
    m_ctrlIo.writeCommand(&CMD, answ, 2);
    if (m_ctrlIo.getLastErrorMask() == 0) {
        err = answ[0];
        ret = ZeraMControllerIo::cmddone;
    }
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlEMOB::clearErrorStatus()
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
    hw_cmd CMD(hwClearErrorStatus, 0, nullptr, 0);
    m_ctrlIo.writeCommand(&CMD);
    if (m_ctrlIo.getLastErrorMask() != 0)
        return ZeraMControllerIo::cmdexecfault;
    return ZeraMControllerIo::cmddone;
}
