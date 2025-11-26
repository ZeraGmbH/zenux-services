#include "mocki2cctrlbootloader.h"
#include "controllerpersitentdata.h"

MockI2cCtrlBootloader::MockI2cCtrlBootloader(qint8 muxChannel, bool replyOnBootloaderCommands) :
    m_muxChannel(muxChannel),
    m_replyOnBootloaderCommands(replyOnBootloaderCommands)
{
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

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlBootloader::genResponse()
{
    if(ControllerPersitentData::isHotControllerAvailable(m_muxChannel) && m_replyOnBootloaderCommands)
        return ZeraMControllerIo::atmelRM::cmddone;
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}
