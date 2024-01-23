#include "i2cctrlaccu.h"

I2cCtrlAccu::I2cCtrlAccu(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwSetTestModes = 0x0042,
    hwGetAccumulatorStatus = 0x0210,
    hwGetAccuStateOfCharge = 0x0211,
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlAccu::readAccuStatus(quint8 &stat)
{
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIo::cmdexecfault;
    quint8 answ[2];
    hw_cmd CMD(hwGetAccumulatorStatus, 0, nullptr, 0);
    m_ctrlIo.writeCommand(&CMD, answ, 2);
    if(m_ctrlIo.getLastErrorMask() == 0) {
        stat = answ[0];
        ret = ZeraMControllerIo::cmddone;
    }
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlAccu::readAccuStateOfCharge(quint8 &charge)
{
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIo::cmdexecfault;
    quint8 answ[2];
    hw_cmd CMD(hwGetAccuStateOfCharge, 0, nullptr, 0);
    m_ctrlIo.writeCommand(&CMD, answ, 2);
    if(m_ctrlIo.getLastErrorMask() == 0) {
        charge = answ[0];
        ret = ZeraMControllerIo::cmddone;
    }
    return ret;
}
