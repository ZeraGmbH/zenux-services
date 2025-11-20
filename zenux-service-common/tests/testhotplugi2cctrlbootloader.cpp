#include "testhotplugi2cctrlbootloader.h"

TestHotplugI2cCtrlBootloader::TestHotplugI2cCtrlBootloader(AbstractFactoryI2cCtrl::ControllerTypes ctrlType,
                                                           quint8 muxChannel,
                                                           bool replyOnBootloaderCommands) :
    m_replyOnBootloaderCommands(replyOnBootloaderCommands)
{
}

ZeraMControllerIoTemplate::atmelRM TestHotplugI2cCtrlBootloader::genResponse()
{
    if(m_replyOnBootloaderCommands)
        return ZeraMControllerIo::atmelRM::cmddone;
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM TestHotplugI2cCtrlBootloader::bootloaderLoadFlash(cIntelHexFileIO &ihxFIO)
{
    Q_UNUSED(ihxFIO)
    return genResponse();
}

ZeraMControllerIoTemplate::atmelRM TestHotplugI2cCtrlBootloader::bootloaderStartProgram()
{
    return genResponse();
}
