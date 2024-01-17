#include "i2cctrleeprompermission.h"

I2cCtrlEepromPermission::I2cCtrlEepromPermission(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwGetFlashWriteAccess=0x100B
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlEepromPermission::getEEPROMAccessEnable(bool &enable)
{
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIo::cmdexecfault;
    enable = false; // default
    hw_cmd CMD(hwGetFlashWriteAccess, 0, nullptr, 0);
    quint8 answ[2];
    m_ctrlIo.writeCommand(&CMD, answ, 2);
    if(m_ctrlIo.getLastErrorMask() == 0) {
        enable = (answ[0] != 0);
        ret = ZeraMControllerIo::cmddone;
    }
    return ret;
}
