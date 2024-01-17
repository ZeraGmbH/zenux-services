#include "i2cctrlcriticalstatus.h"

I2cCtrlCriticalStatus::I2cCtrlCriticalStatus(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwGetCritStat = 0x0200,
    hwResetCritStat = 0x0201,
    hwSetIntMask = 0x0202,
    hwGetIntMask = 0x0203
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlCriticalStatus::writeIntMask(quint16 mask)
{
    quint8 PAR[2];
    PAR[0] = (mask >> 8) & 255;
    PAR[1] = mask & 255;
    hw_cmd CMD(hwSetIntMask, 0, PAR, 2);
    m_ctrlIo.writeCommand(&CMD);
    return m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlCriticalStatus::readIntMask(quint16 &mask)
{
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIo::cmdexecfault;
    quint8 answ[3];
    hw_cmd CMD(hwGetIntMask, 0, nullptr, 0);
    m_ctrlIo.writeCommand(&CMD, answ, 3);
    if(m_ctrlIo.getLastErrorMask() == 0) {
        mask = static_cast<quint16>(answ[0] << 8) + answ[1];
        ret = ZeraMControllerIo::cmddone;
    }
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlCriticalStatus::readCriticalStatus(quint16 &stat)
{
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIo::cmdexecfault;
    quint8 answ[3];
    hw_cmd CMD(hwGetCritStat, 0, nullptr, 0);
    m_ctrlIo.writeCommand(&CMD, answ, 3);
    if(m_ctrlIo.getLastErrorMask() == 0) {
        stat = (static_cast<quint16>(answ[0]) << 8) + answ[1];
        ret = ZeraMControllerIo::cmddone;
    }
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlCriticalStatus::resetCriticalStatus(quint16 stat)
{
    quint8 PAR[2];
    PAR[0] = (stat >> 8) & 255;
    PAR[1] = stat & 255;
    hw_cmd CMD(hwResetCritStat, 0, PAR, 2);
    m_ctrlIo.writeCommand(&CMD);
    return m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
}
