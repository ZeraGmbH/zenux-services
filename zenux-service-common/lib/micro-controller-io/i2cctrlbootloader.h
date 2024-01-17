#ifndef I2CCTRLBOOTLOADER_H
#define I2CCTRLBOOTLOADER_H

#include "abstracti2ccontrollers.h"

class I2cCtrlBootloader : public AbstractI2cCtrlBootloader
{
public:
    I2cCtrlBootloader(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM bootloaderLoadFlash(cIntelHexFileIO& ihxFIO) override;
    ZeraMControllerIo::atmelRM bootloaderStartProgram() override;
private:
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLBOOTLOADER_H
