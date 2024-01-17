#include "mocki2cctrlbootloader.h"

MockI2cCtrlBootloader::MockI2cCtrlBootloader()
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlBootloader::bootloaderStartProgram()
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlBootloader::bootloaderLoadFlash(cIntelHexFileIO &ihxFIO)
{
    Q_UNUSED(ihxFIO)
    return ZeraMControllerIo::atmelRM::cmddone;
}
