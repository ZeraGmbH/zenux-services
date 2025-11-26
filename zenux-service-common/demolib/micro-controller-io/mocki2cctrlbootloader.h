#ifndef MOCKI2CCTRLBOOTLOADER_H
#define MOCKI2CCTRLBOOTLOADER_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlBootloader : public AbstractI2cCtrlBootloader
{
public:
    MockI2cCtrlBootloader(qint8 muxChannel, bool replyOnBootloaderCommands = true);
    ZeraMControllerIo::atmelRM bootloaderStartProgram() override;
    ZeraMControllerIo::atmelRM bootloaderLoadFlash(cIntelHexFileIO& ihxFIO) override;
private:
    ZeraMControllerIo::atmelRM genResponse();

    qint8 m_muxChannel;
    bool m_replyOnBootloaderCommands;
};

#endif // MOCKI2CCTRLBOOTLOADER_H
