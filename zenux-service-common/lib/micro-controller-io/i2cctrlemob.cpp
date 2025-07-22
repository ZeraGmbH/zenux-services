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
    hwSendPushbuttonPress = 0x0210
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlEMOB::sendPushbuttonPress()
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);

    ZeraMControllerIo::atmelRM ret;

    quint32 ui32Temp = 0;
    quint8 ba[4];
    ba[0] = (ui32Temp >> 24) & 0xFF;
    ba[1] = (ui32Temp >> 16) & 0xFF;
    ba[2] = (ui32Temp >> 8) & 0xFF;
    ba[3] = (ui32Temp & 0xFF);
    hw_cmd CMD(hwSendPushbuttonPress, 0, ba, 4);
    m_ctrlIo.writeCommand(&CMD);
    ret = m_ctrlIo.getErrorMaskText() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
    return ret;
}
