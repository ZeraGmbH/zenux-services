#ifndef TESTHOTPLUGI2CCTRLBOOTLOADER_H
#define TESTHOTPLUGI2CCTRLBOOTLOADER_H

#include "abstractalli2ccontrollers.h"
#include "abstractfactoryi2cctrl.h"

class TestHotplugI2cCtrlBootloader : public AbstractI2cCtrlBootloader
{
public:
    TestHotplugI2cCtrlBootloader(AbstractFactoryI2cCtrl::ControllerTypes ctrlType,
                                 quint8 muxChannel,
                                 bool replyOnBootloaderCommands = true);
    ZeraMControllerIo::atmelRM bootloaderStartProgram() override;
    ZeraMControllerIo::atmelRM bootloaderLoadFlash(cIntelHexFileIO& ihxFIO) override;
private:
    ZeraMControllerIo::atmelRM genResponse();
    bool m_replyOnBootloaderCommands = true;
};

#endif // TESTHOTPLUGI2CCTRLBOOTLOADER_H
