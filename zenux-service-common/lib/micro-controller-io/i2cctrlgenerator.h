#ifndef I2CCTRLGENERATOR_H
#define I2CCTRLGENERATOR_H

#include "abstractalli2ccontrollers.h"
#include "sensesettings.h"

class I2cCtrlGenerator : public AbstractI2cCtrlGenerator
{
public:
    I2cCtrlGenerator(cSenseSettingsPtr senseSettings, const QString &deviceNodeName, quint8 i2cAddress, quint8 debugLevel);

    ZeraMControllerIo::atmelRM readSourceModeOn(QStringList &channelMNamesModeOnRead) override;
    ZeraMControllerIo::atmelRM sendSourceModeOn(const QStringList &channelMNamesModeOn) override;

    ZeraMControllerIo::atmelRM readSourceOn(QStringList &channelMNamesOnRead) override;
    ZeraMControllerIo::atmelRM sendSourceOn(const QStringList &channelMNamesOn) override;

    ZeraMControllerIo::atmelRM setRangeByAmplitude(const QString &channelMName, float amplitude) override;
    ZeraMControllerIo::atmelRM readRange(const QString& channelMName, quint8& range) override;
    ZeraMControllerIo::atmelRM setRange(const QString& channelMName, quint8 range) override;

    ZeraMControllerIo::atmelRM getDspAmplitude(const QString& channelMName, float &amplitude) override;
    ZeraMControllerIo::atmelRM setDspAmplitude(const QString& channelMName, float amplitude) override;
    ZeraMControllerIo::atmelRM setDspFrequency(const QString& channelMName, float frequency) override;
    ZeraMControllerIo::atmelRM getDspFrequency(const QString& channelMName, float &frequency) override;
    ZeraMControllerIo::atmelRM setDspAngle(const QString& channelMName, float angleDeg) override;
    ZeraMControllerIo::atmelRM getDspAngle(const QString& channelMName, float &angleDeg) override;

    ZeraMControllerIo::atmelRM tunnelToDsp(const QString& channelMName, const QByteArray &cmd, QByteArray &dataReturned) override;

    static quint8 getBitmask(cSenseSettingsPtr senseSettings, const QStringList &channelMNames);
    static QStringList getChannelMNamesFromMask(cSenseSettingsPtr senseSettings, quint8 mask);
    static quint8 getControllerInternalChannelNo(cSenseSettingsPtr senseSettings, const QString &channelMName);

private:
    cSenseSettingsPtr m_senseSettings;
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLGENERATOR_H
