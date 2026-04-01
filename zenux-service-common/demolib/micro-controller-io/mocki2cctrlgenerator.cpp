#include "mocki2cctrlgenerator.h"

MockI2cCtrlGenerator::MockI2cCtrlGenerator(ControllerPersitentData::TPersitentControllerData &persistentData) :
    m_channelMNamesModeOn(persistentData.m_generatorMNamesModeOn),
    m_channelMNamesOn(persistentData.m_generatorMNamesOn),
    m_generatorRangeMap(persistentData.m_generatorRangeMap),
    m_dspAmplitudeMap(persistentData.m_generatorDspAmplitudeMap),
    m_dspFrequencyMap(persistentData.m_generatorDspFrequencyMap)
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

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::getDspAmplitude(const QString &channelMName, float &amplitude)
{
    amplitude = m_dspAmplitudeMap[channelMName];
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::setDspAmplitude(const QString &channelMName, float amplitude)
{
    m_dspAmplitudeMap[channelMName] = amplitude;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::getDspFrequency(const QString &channelMName, float &frequency)
{
    frequency = m_dspFrequencyMap[channelMName];
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::setDspFrequency(const QString &channelMName, float frequency)
{
    m_dspFrequencyMap[channelMName] = frequency;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::tunnelToDsp(const QString& channelMName, const QByteArray &cmd, QByteArray &response)
{
    Q_UNUSED(channelMName)
    Q_UNUSED(cmd)
    Q_UNUSED(response)
    return ZeraMControllerIo::cmddone;
}
