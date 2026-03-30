#include "i2cctrlgenerator.h"
#include <QDataStream>
#include <QIODevice>

I2cCtrlGenerator::I2cCtrlGenerator(cSenseSettingsPtr senseSettings, const QString &deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_senseSettings(senseSettings),
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwSendSetSourceMode = 0x1210,
    hwSendGetSourceMode = 0x1211,
    hwSendSetSourceOn = 0x1212,
    hwSendGetSourceOn = 0x1213,
    hwSendSetSourceRangeByLevel = 0x1206,
    hwSendSetSourceRange = 0x1200,
    hwSendGetSourceRange = 0x1201,
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlGenerator::readSourceModeOn(QStringList &channelMNamesModeOnRead)
{
    hw_cmd CMD(hwSendGetSourceMode, 0, nullptr, 0);
    quint8 answ[2];
    m_ctrlIo.writeCommand(&CMD, answ, 2);
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIo::cmdexecfault;
    if(m_ctrlIo.getLastErrorMask() == 0) {
        quint8 mask = answ[0];
        channelMNamesModeOnRead = getChannelMNamesFromMask(m_senseSettings, mask);
        ret = ZeraMControllerIo::cmddone;
    }
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlGenerator::sendSourceModeOn(const QStringList &channelMNamesModeOn)
{
    quint8 ba = getBitmask(m_senseSettings, channelMNamesModeOn);
    hw_cmd CMD(hwSendSetSourceMode, 0, &ba, 1);
    m_ctrlIo.writeCommand(&CMD);
    ZeraMControllerIo::atmelRM ret = m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlGenerator::readSourceOn(QStringList &channelMNamesOnRead)
{
    hw_cmd CMD(hwSendGetSourceOn, 0, nullptr, 0);
    quint8 answ[2];
    m_ctrlIo.writeCommand(&CMD, answ, 2);
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIo::cmdexecfault;
    if(m_ctrlIo.getLastErrorMask() == 0) {
        quint8 mask = answ[0];
        channelMNamesOnRead = getChannelMNamesFromMask(m_senseSettings, mask);
        ret = ZeraMControllerIo::cmddone;
    }
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlGenerator::sendSourceOn(const QStringList &channelMNamesOn)
{
    quint8 ba = getBitmask(m_senseSettings, channelMNamesOn);
    hw_cmd CMD(hwSendSetSourceOn, 0, &ba, 1);
    m_ctrlIo.writeCommand(&CMD);
    ZeraMControllerIo::atmelRM ret = m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlGenerator::setRangeByAmplitude(const QString &channelMName, float amplitude)
{
    quint8 controllerChannelNo = getControllerInternalChannelNo(m_senseSettings, channelMName);
    QByteArray ba = convertFloat(amplitude);
    hw_cmd CMD(hwSendSetSourceRangeByLevel, controllerChannelNo, reinterpret_cast<quint8*>(ba.data()), ba.size());
    m_ctrlIo.writeCommand(&CMD);
    ZeraMControllerIo::atmelRM ret = m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlGenerator::setRange(const QString &channelMName, quint8 range)
{
    quint8 controllerChannelNo = getControllerInternalChannelNo(m_senseSettings, channelMName);
    hw_cmd CMD(hwSendSetSourceRange, controllerChannelNo, &range, 1);
    m_ctrlIo.writeCommand(&CMD);
    ZeraMControllerIo::atmelRM ret = m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlGenerator::readRange(const QString &channelMName, quint8 &range)
{
    quint8 controllerChannelNo = getControllerInternalChannelNo(m_senseSettings, channelMName);
    hw_cmd CMD(hwSendGetSourceRange, controllerChannelNo, nullptr, 0);
    quint8 answ[2];
    m_ctrlIo.writeCommand(&CMD, answ, 2);
    ZeraMControllerIo::atmelRM ret = m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
    if(ret == ZeraMControllerIo::cmddone)
        range = answ[0];
    return ret;
}

QByteArray I2cCtrlGenerator::convertFloat(float value)
{
    QByteArray memOut;
    QDataStream stream(&memOut, QIODevice::Unbuffered | QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream<<value;
    return memOut;
}

quint8 I2cCtrlGenerator::getBitmask(cSenseSettingsPtr senseSettings, const QStringList &channelMNames)
{
    quint8 mask = 0;
    for (const QString &channelMName : channelMNames) {
        // for now...
        const SenseSystem::cChannelSettings* channelSetting = senseSettings->findChannelSettingByMxName(channelMName);
        if (channelSetting)
            mask |= (1 << (channelSetting->m_nCtrlChannel-1));
    }
    return mask;
}

QStringList I2cCtrlGenerator::getChannelMNamesFromMask(cSenseSettingsPtr senseSettings, quint8 mask)
{
    QStringList channelMNames;
    const QList<SenseSystem::cChannelSettings*> &channelSettings = senseSettings->getChannelSettings();
    for (const SenseSystem::cChannelSettings* channelSetting : channelSettings) {
        if (mask & (1 << (channelSetting->m_nCtrlChannel-1)))
            channelMNames.append(channelSetting->m_nameMx);
    }
    return channelMNames;
}

quint8 I2cCtrlGenerator::getControllerInternalChannelNo(cSenseSettingsPtr senseSettings, const QString &channelMName)
{
    SenseSystem::cChannelSettings* channelSetting = senseSettings->findChannelSettingByMxName(channelMName);
    if (channelSetting)
        return channelSetting->m_nCtrlChannel;
    return 0;
}
