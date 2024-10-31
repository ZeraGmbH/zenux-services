#include "i2cctrlcputemperature.h"


I2cCtrlCpuTemperature::I2cCtrlCpuTemperature(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwSendTemperature = 0x2050
};


ZeraMControllerIoTemplate::atmelRM I2cCtrlCpuTemperature::sendCpuTemperature(float &temperature)
{
    ZeraMControllerIo::atmelRM ret;

    quint32 ui32Temp;
    ui32Temp = (quint32)temperature * 1000;
    quint8 ba[4];
    ba[0] = (ui32Temp >> 24) & 0xFF;
    ba[1] = (ui32Temp >> 16) & 0xFF;
    ba[2] = (ui32Temp >> 8) & 0xFF;
    ba[3] = (ui32Temp & 0xFF);
    hw_cmd CMD(hwSendTemperature, 0, ba, 4);
    m_ctrlIo.writeCommand(&CMD);
    ret = m_ctrlIo.getErrorMaskText() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
    return ret;
}
