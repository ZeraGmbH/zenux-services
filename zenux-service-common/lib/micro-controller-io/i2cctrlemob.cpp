#include "i2cctrlemob.h"
#include <i2cmuxerscopedonoff.h>

I2cCtrlEMOB::I2cCtrlEMOB(QString deviceNodeName,
                         quint8 i2cAddress,
                         quint8 i2cAddressMux,
                         quint8 muxChannel,
                         quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel),
    m_i2cMuxer(I2cMultiplexerFactory::createPCA9547Muxer(deviceNodeName, i2cAddressMux, muxChannel))
{
}

enum hw_cmdcode
{
    hwSendPushbuttonPress = 0x0041
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlEMOB::sendPushbuttonPress()
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
    hw_cmd CMD(hwSendPushbuttonPress, 0, nullptr, 0);
    m_ctrlIo.writeCommand(&CMD);
    if (m_ctrlIo.getLastErrorMask() != 0)
        return ZeraMControllerIo::cmdexecfault;
    return ZeraMControllerIo::cmddone;
}
