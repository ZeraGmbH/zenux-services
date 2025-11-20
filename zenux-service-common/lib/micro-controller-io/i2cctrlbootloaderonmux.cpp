#include "i2cctrlbootloaderonmux.h"
#include "i2cmuxerscopedonoff.h"

I2cCtrlBootloaderOnMux::I2cCtrlBootloaderOnMux(QString deviceNodeName,
                                               quint8 i2cAddress,
                                               quint8 i2cAddressMux,
                                               quint8 muxChannel,
                                               quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel),
    m_i2cMuxer(I2cMultiplexerFactory::createPCA9547Muxer(deviceNodeName, i2cAddressMux, muxChannel))
{
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlBootloaderOnMux::bootloaderStartProgram()
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
    return m_ctrlIo.bootloaderStartProgram();
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlBootloaderOnMux::bootloaderLoadFlash(cIntelHexFileIO &ihxFIO)
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
    return m_ctrlIo.bootloaderLoadFlash(ihxFIO);
}
