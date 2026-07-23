#ifndef I2CCTRLBOOTLOADERONMUX_H
#define I2CCTRLBOOTLOADERONMUX_H

#include "abstractalli2ccontrollers.h"
#include <i2cmultiplexerfactory.h>

class I2cCtrlBootloaderOnMux : public AbstractI2cCtrlBootloader
{
public:
    I2cCtrlBootloaderOnMux(QString deviceNodeName, quint8 i2cAddress,
                           quint8 i2cAddressMux, qint8 muxChannel,
                           quint8 debugLevel);
    ZeraMControllerIo::atmelRM bootloaderLoadFlash(cIntelHexFileIO& ihxFIO) override;
    ZeraMControllerIo::atmelRM bootloaderStartProgram() override;
private:
    ZeraMControllerIo m_ctrlIo;
    I2cMuxerInterface::Ptr m_i2cMuxer;
};

#endif // I2CCTRLBOOTLOADERONMUX_H
