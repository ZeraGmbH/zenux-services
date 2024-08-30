#include "i2csendtemp.h"

I2cSendTemp::I2cSendTemp(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

quint16 hwSendTemp = 0x2050;

ZeraMControllerIoTemplate::atmelRM I2cSendTemp::sendTemp(float temperature)
{
    uint32_t uiTemp = (uint32_t) temperature;   // is already in mV
    quint8 PAR[4];
    PAR[0] = (uiTemp >> 24) & 255;
    PAR[1] = (uiTemp >> 16) & 255;
    PAR[2] = (uiTemp >> 8)  & 255;
    PAR[3] = (uiTemp & 255);
    hw_cmd CMD(hwSendTemp, 0, PAR, 4);
    m_ctrlIo.writeCommand(&CMD);
    quint32 errorMask = m_ctrlIo.getLastErrorMask();
    ZeraMControllerIo::atmelRM ret = errorMask == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
    if(ret != ZeraMControllerIo::cmddone)
        qWarning("Send Temperatur to system uC failed");
    return ret;
}
