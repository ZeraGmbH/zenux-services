#ifndef DEMOI2CCTRLBOOTLOADER_H
#define DEMOI2CCTRLBOOTLOADER_H

#include "abstractalli2ccontrollers.h"

class DemoI2cCtrlBootloader : public AbstractI2cCtrlBootloader
{
public:
    ZeraMControllerIo::atmelRM bootloaderStartProgram() override;
    ZeraMControllerIo::atmelRM bootloaderLoadFlash(cIntelHexFileIO& ihxFIO) override;
};

#endif // DEMOI2CCTRLBOOTLOADER_H
