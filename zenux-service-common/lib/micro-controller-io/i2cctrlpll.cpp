#include "i2cctrlpll.h"

I2cCtrlPll::I2cCtrlPll(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwSetPLLChannel = 0x1002,
    hwGetPLLChannel = 0x1003,
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlPll::setPLLChannel(quint8 chn)
{
    hw_cmd CMD(hwSetPLLChannel, 0, &chn, 1);
    m_ctrlIo.writeCommand(&CMD);
    return m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlPll::readPLLChannel(quint8 &chn)
{
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIo::cmdexecfault;
    chn = 0; // default AC
    hw_cmd CMD(hwGetPLLChannel, 0, nullptr, 0);
    quint8 answ[2];
    m_ctrlIo.writeCommand(&CMD, answ, 2);
    if(m_ctrlIo.getLastErrorMask() == 0) {
        chn = answ[0];
        ret = ZeraMControllerIo::cmddone;
    }
    return ret;
}
