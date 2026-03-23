#include "mocki2cctrlgenerator.h"

MockI2cCtrlGenerator::MockI2cCtrlGenerator()
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::sendSourceAmplitudeChangeRange(float amplitude, ChannelDefinitions channel)
{
    Q_UNUSED(amplitude)
    Q_UNUSED(channel)
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::sendSourceModeOn(QList<ChannelDefinitions> channelsOn)
{
    Q_UNUSED(channelsOn)
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::sendSourceOn(QList<ChannelDefinitions> channelsOn)
{
    Q_UNUSED(channelsOn)
    return ZeraMControllerIo::cmddone;
}
