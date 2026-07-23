#include "i2cctrlbootloader.h"

I2cCtrlBootloader::I2cCtrlBootloader(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlBootloader::bootloaderLoadFlash(cIntelHexFileIO &ihxFIO)
{
    return m_ctrlIo.bootloaderLoadFlash(ihxFIO);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlBootloader::bootloaderStartProgram()
{
    return m_ctrlIo.bootloaderStartProgram();
}
