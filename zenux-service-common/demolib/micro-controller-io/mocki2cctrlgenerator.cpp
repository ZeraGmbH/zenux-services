#include "mocki2cctrlgenerator.h"

MockI2cCtrlGenerator::MockI2cCtrlGenerator()
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::sendSourceModeOn(QList<ChannelDefinitions> channelsOn)
{
    Q_UNUSED(channelsOn)
    return ZeraMControllerIo::cmddone;
}
