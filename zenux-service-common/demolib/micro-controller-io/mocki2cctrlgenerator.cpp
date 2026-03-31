#include "mocki2cctrlgenerator.h"

MockI2cCtrlGenerator::MockI2cCtrlGenerator(QStringList &channelMNamesModeOn,
                                           QStringList &channelMNamesOn,
                                           ControllerPersitentData::RangeMap &generatorRangeMap) :
    m_channelMNamesModeOn(channelMNamesModeOn),
    m_channelMNamesOn(channelMNamesOn),
    m_generatorRangeMap(generatorRangeMap)
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

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::setRangeByAmplitude(const QString &channelMName, float amplitude)
{
    Q_UNUSED(amplitude)
    Q_UNUSED(channelMName)
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::setRange(const QString &channelMName, quint8 range)
{
    m_generatorRangeMap[channelMName] = range;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::readRange(const QString &channelMName, quint8 &range)
{
    range = m_generatorRangeMap[channelMName];
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::tunnelToDsp(const QString &channelMName, DspTunnelParamAndResponse &dspIo)
{
    Q_UNUSED(channelMName)
    Q_UNUSED(dspIo)
    return ZeraMControllerIo::cmddone;
}
