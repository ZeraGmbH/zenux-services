#include "mocki2cctrlbootloader.h"

MockI2cCtrlBootloader::MockI2cCtrlBootloader(bool replyOnBootloaderCommands) :
    m_replyOnBootloaderCommands(replyOnBootloaderCommands)
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlBootloader::genResponse()
{
    if(m_replyOnBootloaderCommands)
        return ZeraMControllerIo::atmelRM::cmddone;
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlBootloader::bootloaderStartProgram()
{
    return genResponse();
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlBootloader::bootloaderLoadFlash(cIntelHexFileIO &ihxFIO)
{
    Q_UNUSED(ihxFIO)
    return genResponse();
}
