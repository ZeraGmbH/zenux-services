#include "demoi2cctrlbootloader.h"

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlBootloader::bootloaderStartProgram()
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlBootloader::bootloaderLoadFlash(cIntelHexFileIO &ihxFIO)
{
    Q_UNUSED(ihxFIO)
    return ZeraMControllerIo::atmelRM::cmddone;
}

