#ifndef TESTI2CCTRLBOOTLOADER_H
#define TESTI2CCTRLBOOTLOADER_H

#include "abstractfactoryi2cctrl.h"

class TestI2cCtrlBootloader : public AbstractI2cCtrlBootloader
{
public:
    TestI2cCtrlBootloader(AbstractFactoryI2cCtrl::ControllerTypes ctrlType,
                          quint8 muxChannel,
                          bool replyOnBootloaderCommands = true);
    ZeraMControllerIo::atmelRM bootloaderStartProgram() override;
    ZeraMControllerIo::atmelRM bootloaderLoadFlash(cIntelHexFileIO& ihxFIO) override;
private:
    ZeraMControllerIo::atmelRM genResponse();
    bool m_replyOnBootloaderCommands = true;
};

#endif // TESTI2CCTRLBOOTLOADER_H
