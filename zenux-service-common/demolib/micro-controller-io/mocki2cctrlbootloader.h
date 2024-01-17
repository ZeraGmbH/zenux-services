#ifndef MOCKI2CCTRLBOOTLOADER_H
#define MOCKI2CCTRLBOOTLOADER_H

#include <abstracti2ccontrollers.h>

class MockI2cCtrlBootloader : public AbstractI2cCtrlBootloader
{
public:
    MockI2cCtrlBootloader();
    ZeraMControllerIo::atmelRM bootloaderStartProgram() override;
    ZeraMControllerIo::atmelRM bootloaderLoadFlash(cIntelHexFileIO& ihxFIO) override;
};

#endif // MOCKI2CCTRLBOOTLOADER_H
