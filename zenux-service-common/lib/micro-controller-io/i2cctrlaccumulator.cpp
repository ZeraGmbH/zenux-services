#include "i2cctrlaccumulator.h"

I2cCtrlAccumulator::I2cCtrlAccumulator(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwSetTestModes = 0x0042,
    hwGetAccumulatorStatus = 0x0210,
    hwGetAccuStateOfCharge = 0x0211,
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlAccumulator::readAccumulatorStatus(quint8 &stat)
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

ZeraMControllerIoTemplate::atmelRM I2cCtrlAccumulator::readAccuStateOfCharge(quint8 &charge)
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

ZeraMControllerIoTemplate::atmelRM I2cCtrlAccumulator::enableTestMode(qint32 testBits)
{
    quint8 PAR[4];
    PAR[0] = (testBits >> 24) & 255;
    PAR[1] = (testBits >> 16) & 255;
    PAR[2] = (testBits >> 8) & 255;
    PAR[3] = testBits & 255;
    hw_cmd CMD(hwSetTestModes, 0, PAR, 4);
    m_ctrlIo.writeCommand(&CMD);
    return m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
}