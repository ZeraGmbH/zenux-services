#include "i2cctrlclampstatus.h"

I2cCtrlClampStatus::I2cCtrlClampStatus(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwGetClampStatus = 0x0204
};


ZeraMControllerIoTemplate::atmelRM I2cCtrlClampStatus::readClampStatus(quint16 &stat)
{
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIo::cmdexecfault;
    quint8 answ[2];
    hw_cmd CMD(hwGetClampStatus, 0, nullptr, 0);
    m_ctrlIo.writeCommand(&CMD, answ, 2);
    if(m_ctrlIo.getLastErrorMask() == 0) {
        stat = answ[0];
        ret = ZeraMControllerIo::cmddone;
    }
    return ret;
}
