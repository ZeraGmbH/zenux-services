#include "testi2cctrlbootloader.h"

TestI2cCtrlBootloader::TestI2cCtrlBootloader(AbstractFactoryI2cCtrl::ControllerTypes ctrlType,
                                             quint8 muxChannel,
                                             bool replyOnBootloaderCommands) :
    m_replyOnBootloaderCommands(replyOnBootloaderCommands)
{
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlBootloader::genResponse()
{
    if(m_replyOnBootloaderCommands)
        return ZeraMControllerIo::atmelRM::cmddone;
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlBootloader::bootloaderStartProgram()
{
    return genResponse();
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlBootloader::bootloaderLoadFlash(cIntelHexFileIO &ihxFIO)
{
    Q_UNUSED(ihxFIO)
    return genResponse();
}
