#include "mocki2cctrlgenerator.h"

MockI2cCtrlGenerator::MockI2cCtrlGenerator(ControllerPersitentData::TPersitentControllerData &persistentData) :
    m_channelMNamesModeOn(persistentData.m_generatorMNamesModeOn),
    m_channelMNamesOn(persistentData.m_generatorMNamesOn),
    m_generatorRangeMap(persistentData.m_generatorRangeMap),
    m_dspAmplitudeMap(persistentData.m_generatorDspAmplitudeMap),
    m_dspFrequencyMap(persistentData.m_generatorDspFrequencyMap),
    m_dspAngleMap(persistentData.m_generatorDspAngleMap)
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::readSourceModeOn(QStringList &channelMNamesModeOnRead)
{
    channelMNamesModeOnRead = m_channelMNamesModeOn;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::sendSourceModeOn(const QStringList &channelMNamesModeOn)
{
    if (canSwitchOffSourceMode(channelMNamesModeOn)) {
        m_channelMNamesModeOn = channelMNamesModeOn;
        return ZeraMControllerIo::cmddone;
    }
    return ZeraMControllerIo::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::readSourceOn(QStringList &channelMNamesOnRead)
{
    channelMNamesOnRead = m_channelMNamesOn;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::sendSourceOn(const QStringList &channelMNamesOn)
{
    if (canSwitchOn(channelMNamesOn)) {
        m_channelMNamesOn = channelMNamesOn;
        return ZeraMControllerIo::cmddone;
    }
    return ZeraMControllerIo::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::readRange(const QString &channelMName, quint8 &range)
{
    range = m_generatorRangeMap[channelMName];
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::setRangeByAmplitude(const QString &channelMName, float amplitude)
{
    Q_UNUSED(amplitude)
    Q_UNUSED(channelMName)
    if (canChangeRange(channelMName))
        return ZeraMControllerIo::cmddone;
    return ZeraMControllerIo::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::setRange(const QString &channelMName, quint8 range)
{
    if (canChangeRange(channelMName)) {
        m_generatorRangeMap[channelMName] = range;
        return ZeraMControllerIo::cmddone;
    }
    return ZeraMControllerIo::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::setDspAmplitude(const QString &channelMName, float amplitude)
{
    m_dspAmplitudeMap[channelMName] = amplitude;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::getDspAmplitude(const QString &channelMName, float &amplitude)
{
    amplitude = m_dspAmplitudeMap[channelMName];
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::setDspFrequency(const QString &channelMName, float frequency)
{
    m_dspFrequencyMap[channelMName] = frequency;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::getDspFrequency(const QString &channelMName, float &frequency)
{
    frequency = m_dspFrequencyMap[channelMName];
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::setDspAngle(const QString &channelMName, float angleDeg)
{
    m_dspAngleMap[channelMName] = angleDeg;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::getDspAngle(const QString &channelMName, float &angleDeg)
{
    angleDeg = m_dspAngleMap[channelMName];
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlGenerator::tunnelToDsp(const QString& channelMName, const QByteArray &cmd, QByteArray &response)
{
    Q_UNUSED(channelMName)
    Q_UNUSED(cmd)
    Q_UNUSED(response)
    return ZeraMControllerIo::cmddone;
}

// At the time of writing there are 3 constraints on channels:

// 1. SourceRanges cannot be changed when source mode is not active
//    https://github.com/ZeraGmbH/ADW5859/blob/9cff2865b3f6bb9032be12a915f2cf8ae058d852/ADW5859/Commands.c#L501
bool MockI2cCtrlGenerator::canChangeRange(const QString &channelMName)
{
    if (m_channelMNamesModeOn.contains(channelMName))
        return true;
    qWarning("Cannot change source range on %s - source mode is not on!", qPrintable(channelMName));
    return false;
}

// 2. Changing from source mode to measurement mode is not allowed if source channel is on
//    https://github.com/ZeraGmbH/ADW5859/blob/9cff2865b3f6bb9032be12a915f2cf8ae058d852/ADW5859/Commands.c#L832
bool MockI2cCtrlGenerator::canSwitchOffSourceMode(const QStringList &channelMNamesModeOn)
{
    QStringList notAllowedChannels;
    for (const QString &channelMNameOn : qAsConst(m_channelMNamesOn)) { // assume a channel on has source mode on - see 3.
        if (!channelMNamesModeOn.contains(channelMNameOn))
            notAllowedChannels.append(channelMNameOn);
    }
    if (notAllowedChannels.isEmpty())
        return true;
    qWarning("Cannot switch off source mode for channels %s. They are currently switched on!",
             qPrintable(notAllowedChannels.join(" / ")));
    return false;
}

// 3. Switch on is allowed only on channels with source mode on
//    https://github.com/ZeraGmbH/ADW5859/blob/9cff2865b3f6bb9032be12a915f2cf8ae058d852/ADW5859/Commands.c#L856
bool MockI2cCtrlGenerator::canSwitchOn(const QStringList &channelMNamesModeOn)
{
    QStringList notAllowedChannels;
    for (const QString &channelMNameOn : qAsConst(channelMNamesModeOn)) {
        bool sourceModeIsOn = m_channelMNamesModeOn.contains(channelMNameOn);
        if (!sourceModeIsOn)
            notAllowedChannels.append(channelMNameOn);
    }
    if (notAllowedChannels.isEmpty())
        return true;
    qWarning("Cannot switch on channels %s - they are not in source mode!",
             qPrintable(notAllowedChannels.join(" / ")));
    return false;
}

