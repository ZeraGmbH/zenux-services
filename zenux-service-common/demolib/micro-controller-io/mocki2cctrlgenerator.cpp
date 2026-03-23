#include "mocki2cctrlgenerator.h"

MockI2cCtrlGenerator::MockI2cCtrlGenerator()
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::sendSourceAmplitudeChangeRange(float amplitude, const QString &channelMName)
{
    Q_UNUSED(amplitude)
    Q_UNUSED(channelMName)
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::sendSourceModeOn(const QStringList &channelMNamesOn)
{
    Q_UNUSED(channelMNamesOn)
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::sendSourceOn(const QStringList &channelMNamesOn)
{
    Q_UNUSED(channelMNamesOn)
    return ZeraMControllerIo::cmddone;
}
