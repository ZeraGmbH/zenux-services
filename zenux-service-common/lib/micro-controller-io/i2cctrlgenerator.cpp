#include "i2cctrlgenerator.h"
#include <QDataStream>
#include <QIODevice>

I2cCtrlGenerator::I2cCtrlGenerator(const QString &deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwSendSetSourceMode = 0x1210,
    hwSendSetSourceOn = 0x1212,
    hwSendSetSourceRangeByLevel = 0x1206
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlGenerator::sendSourceModeOn(QList<ChannelDefinitions> channelsOn)
{
    quint8 ba = getBitmask(channelsOn);
    hw_cmd CMD(hwSendSetSourceMode, 0, &ba, 1);
    m_ctrlIo.writeCommand(&CMD);
    ZeraMControllerIo::atmelRM ret = m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlGenerator::sendSourceOn(QList<ChannelDefinitions> channelsOn)
{
    quint8 ba = getBitmask(channelsOn);
    hw_cmd CMD(hwSendSetSourceOn, 0, &ba, 1);
    m_ctrlIo.writeCommand(&CMD);
    ZeraMControllerIo::atmelRM ret = m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlGenerator::sendSourceAmplitudeChangeRange(float amplitude, ChannelDefinitions channel)
{
    quint8 controllerChannelNo = getControllerInternalChannelNo(channel);
    QByteArray ba = convertFloat(amplitude);
    hw_cmd CMD(hwSendSetSourceRangeByLevel, controllerChannelNo, reinterpret_cast<quint8*>(ba.data()), ba.size());
    m_ctrlIo.writeCommand(&CMD);
    ZeraMControllerIo::atmelRM ret = m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
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

quint8 I2cCtrlGenerator::getBitmask(const QList<ChannelDefinitions> &channels)
{
    quint8 mask = 0;
    for (ChannelDefinitions channel : channels) {
        switch(channel) {
        case UL1:
            mask |= (1<<0);
            break;
        case UL2:
            mask |= (1<<1);
            break;
        case UL3:
            mask |= (1<<2);
            break;
        case ULAUX:
            mask |= (1<<3);
            break;
        case IL1:
            mask |= (1<<4);
            break;
        case IL2:
            mask |= (1<<5);
            break;
        case IL3:
            mask |= (1<<6);
            break;
        case ILAUX:
            mask |= (1<<7);
            break;
        }
    }
    return mask;
}

quint8 I2cCtrlGenerator::getControllerInternalChannelNo(ChannelDefinitions channel)
{
    // TODO? config / from alias?
    switch(channel) {
    case UL1:
        return 1;
    case UL2:
        return 2;
    case UL3:
        return 3;
    case ULAUX:
        return 4;
    case IL1:
        return 5;
    case IL2:
        return 6;
    case IL3:
        return 7;
    case ILAUX:
        return 8;
    }
    return 0;
}
