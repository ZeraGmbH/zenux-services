#include "i2cctrlranges.h"

I2cCtrlRanges::I2cCtrlRanges(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwSetRange = 0x1100,
    hwGetRange = 0x1101,
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlRanges::readRange(quint8 channel, quint8 &range)
{
    hw_cmd CMD(hwGetRange, channel, nullptr, 0);
    quint8 answ[2];
    m_ctrlIo.writeCommand(&CMD, answ, 2);
    quint32 errorMask = m_ctrlIo.getLastErrorMask();
    ZeraMControllerIo::atmelRM ret = errorMask == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
    if(ret == ZeraMControllerIo::cmddone)
        range = answ[0];
    else
        qWarning("ReadRange failed with ch: %i / mask: %8X failed!",
                 channel, errorMask);
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlRanges::setRange(quint8 channel, quint8 range)
{
    hw_cmd CMD(hwSetRange, channel, &range, 1);
    m_ctrlIo.writeCommand(&CMD);
    quint32 errorMask = m_ctrlIo.getLastErrorMask();
    ZeraMControllerIo::atmelRM ret = errorMask == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
    if(ret != ZeraMControllerIo::cmddone)
        qWarning("SetRange failed with ch: %i / rng: %i / mask: %8X failed!",
                 channel, range, errorMask);
    return ret;
}
