#include "mocki2cctrlgenerator.h"

MockI2cCtrlGenerator::MockI2cCtrlGenerator(QStringList &channelMNamesModeOn, QStringList &channelMNamesOn) :
    m_channelMNamesModeOn(channelMNamesModeOn),
    m_channelMNamesOn(channelMNamesOn)
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::readSourceModeOn(QStringList &channelMNamesModeOnRead)
{
    channelMNamesModeOnRead = m_channelMNamesModeOn;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::sendSourceModeOn(const QStringList &channelMNamesModeOn)
{
    m_channelMNamesModeOn = channelMNamesModeOn;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::readSourceOn(QStringList &channelMNamesOnRead)
{
    channelMNamesOnRead = m_channelMNamesOn;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::sendSourceOn(const QStringList &channelMNamesOn)
{
    m_channelMNamesOn = channelMNamesOn;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::sendSourceAmplitudeChangeRange(float amplitude, const QString &channelMName)
{
    Q_UNUSED(amplitude)
    Q_UNUSED(channelMName)
    return ZeraMControllerIo::cmddone;
}
